Brian West
5003032874
westb2@unlv.nevada.edu


//Brian West CS370 Proj2 v2.2

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <pthread.h>

#define bailout(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define bailout_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info_producer {
    pthread_t handle;	// thread handle
    int id;		// worker thread ID
    int max_count;	// max count
    int done; //A flag in the producer is done (I added this and then didn't use it)
};

struct thread_info_consumer {
    pthread_t handle;	// thread handle
    int id;		// flusher thread ID
    int fd_record;	// log record file descriptor
    long n_wakeups;	// statistics
    long n_itemswritten;; // statistics
};

// 32-byte record
#define RLEN (32)
typedef struct _record {
    char buf[RLEN];
} record_t;


// number of entries in the ring buffer
#define RSIZE (128)
__attribute__((aligned(4096)))	    // GCC keyword to force alignment
record_t items[RSIZE];

/* 
 * struct ring: thread-safe ring buffer to control access to items.
 * 
 * Your task is to implement this data structure and a couple of functions
 * which manipulates the state of ring buffer.
 *
 * Some members fields are already given to you;
 * 'lock' is the obligatory mutex lock to protect the shared state.
 * Semantics of 'head' and 'tail' is similar to BBQ's 'front' and 'nextEmpty'
 * 'all_finished' is a boolean flag to indicate termination condition.
 */
struct ring {
    pthread_mutex_t lock;
    unsigned head;	    // oldest item to pop out
    unsigned tail;	    // empty slot to push to
    int	    all_finished;   // flag set by main() upon all producers joined
   
    // variables I've added
    pthread_cond_t ringFilling; //blocks consumer thread while ring is filling
    pthread_cond_t ringEmptying; //blocks producer threads while the ring is emptying
    int count; //the number of entries in the ring buffer
    
} ring_buf = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .head = 0,
    .tail = 0,
    .ringFilling = PTHREAD_COND_INITIALIZER, // blocks the consumer
    .ringEmptying = PTHREAD_COND_INITIALIZER, //blockes the producers
    .count =0,
    .all_finished = 1, //must always start at one to work with a possible state later in the program
    
};

static inline int is_ring_empty(void) {
    return (ring_buf.head == ring_buf.tail);
}

static inline int is_ring_full(void) {
    return (ring_buf.tail - ring_buf.head) == RSIZE;
}

/*
 * You need to implement this function.
 *
 * flush_ring() consumes log items in the ring buffer by batch-writing
 * out to the log file. Once written, the logs are considered flushed, hence
 * the buffer entries are made available again.
 *
 * This looks similar to BBQ, but the big difference is flush_ring() must 
 * write out the ring buffer using as few write() calls as possible. This
 * can be done because our log records are buffered consecutively in 'items'
 * array. At any given time, at most two write() calls are needed to flush out
 * current ring buffer.
 *
 * Another finer point you have to deal with is that write() call takes long
 * time to finish. This means that you cannot be holding mutex lock when you
 * perform the write() - otherwise producer threads can't write to the ring.
 * It is therefore important to performance to allow producers to write logs
 * to empty slots while flusher thread is performing write().
 *
 * This function should block if ring is empty, and it returns when it has
 * written something out to disk, and the return value is the non-zero number
 * of log entries it has flushed out. 
 * This function can return 0 only if it is indicated that all producer threads
 * have finished, so that the main consumer thread can finish as well. (see 
 * consumer_main()) 
 */
int flush_ring(struct thread_info_consumer* tic) 
{
		
    int n = 0;
    
	
    pthread_mutex_lock(&ring_buf.lock);
    
    while(is_ring_empty()&&ring_buf.all_finished==1){ //makes sure records are present and that the producers are still active 
     	pthread_cond_wait(&ring_buf.ringFilling, &ring_buf.lock); //waiting for ring to not be empty
     }
	
	
	n = write(tic->fd_record,items->buf,sizeof(*items));  
	n = ring_buf.tail-ring_buf.head; //determining the number of items consumed
	
	ring_buf.count=0; //remove count
	ring_buf.tail=0; //I avoided a ring buffer to focus on the multithreading
    pthread_mutex_unlock(&ring_buf.lock);// all data is out of ring buffer
    pthread_cond_signal(&ring_buf.ringEmptying);//need to signal the other condition variable 
	
	
	if(ring_buf.all_finished == 0)
		n = 0;	
    return n;
}

/*
 * consumer_main() is the main for the 'log-flusher' consumer thread.
 *
 * This thread is responsible for consuming the ring buffer entries
 * by writing collected logs to file.
 *
 * It consists only of a simple while() loop calling flush_ring(), which
 * performs actual I/O. If there is no items to flush in ring buffer, 
 * flush_ring() should be blocking, except when all producer threads are
 * finished, upon which flush_ring() returns zero. 
 *
 * You don't need to modify this function.
 */
void* consumer_main(void* arg)
{
    struct thread_info_consumer* tic = (struct thread_info_consumer*)arg;
    int n;

    while ((n = flush_ring(tic)) != 0) {
	tic->n_itemswritten += n;
	tic->n_wakeups++;
    }

    return tic;
}

/*
 * You need to implement this function.
 *
 * write_record() copies the content of record pointed by 'r' to an empty
 * item slot of the ring buffer in a thread-safe way. 
 *
 * This function must block if ring is full, and also must perform necessary
 * signalling action to wake the log-flusher thread. 
 *
 * It returns the size of record. 
 */
ssize_t write_record(struct thread_info_producer* ti, const record_t* r) 
{
	
    pthread_mutex_lock(&ring_buf.lock);
	while(ring_buf.count==127){
		pthread_cond_wait(&ring_buf.ringEmptying,&ring_buf.lock);
	}
	items[ring_buf.tail]=(*r); //writing to ringbuffer
	ring_buf.count++;
	if(ring_buf.tail==RSIZE-1)
		ring_buf.tail=0;
	else
		ring_buf.tail++; //moves tail
    pthread_mutex_unlock(&ring_buf.lock);
    
    
	pthread_cond_signal(&ring_buf.ringFilling); //signaling to consumer
	
    return sizeof(*r);
}

/*
 * producer_main() is the main for the log-producing thread.
 *
 * This is exactly the same as  producer_main() of dumb version.
 * The only difference is write_record() here writes to the 
 * ring buffer instead of invoking write() system call.
 *
 * You don't need to modify this function.
 */
void* producer_main(void* arg)
{
    struct thread_info_producer* ti = (struct thread_info_producer*)arg;
    record_t rec;
    int i, len;
    
    memset(&rec, 0, sizeof(rec));

    for (i = 0; i < ti->max_count; i++) {
    len = snprintf(rec.buf, RLEN, "id:%- 5d, seq:%- 16d ", ti->id, i);
	if (len != RLEN-1) bailout("snprintf clipped");
	rec.buf[RLEN-1] = '\n';
	write_record(ti, &rec);
    }
    ti->done = 1;
    return ti;
}


#define NPRODUCERS (4)

struct thread_info_producer threads_producers[NPRODUCERS];
struct thread_info_consumer threads_consumer;

int main(int argc, char** argv)
{
    int s, i, fd;
    struct thread_info_producer* tip;
    struct thread_info_consumer* tic;

    fd = open("logfile.txt", O_WRONLY | O_CREAT | O_TRUNC,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) bailout("logfile open failed");

    for (i = 0; i < NPRODUCERS; i++) {
	tip = &threads_producers[i];
	tip->id = i;
	tip->max_count = 1024*32;	// write 32K logs per thread
	tip->done =0; //a flag on each instance that signals if the thread has finished
	s = pthread_create(&tip->handle, NULL, &producer_main, tip);
	if (s) bailout_en(s, "pthread_create: producer");
    }

    // create single consumer thread for flushing log buffer
    tic = &threads_consumer;
    tic->id = 0;
    tic->fd_record = fd;
    tic->n_wakeups = 0;
    tic->n_itemswritten = 0;
    s = pthread_create(&tic->handle, NULL, &consumer_main, tic);
    if (s) bailout_en(s, "pthread_create: flusher");

    for (i = 0; i < NPRODUCERS; i++) {
	struct thread_info_producer* ptr;
	tip = &threads_producers[i];
	s = pthread_join(tip->handle, (void**)&ptr); //This makes sure all producer threads are done 
	assert(ptr == tip);
    }
    
    pthread_mutex_lock(&ring_buf.lock); //securing ring_buf
    ring_buf.all_finished = 0; //Setting all finished flag
    pthread_mutex_unlock(&ring_buf.lock);
	pthread_cond_signal(&ring_buf.ringFilling); //Signaling to consumer thread 
    {
	struct thread_info_consumer* ptr;
	s = pthread_join(tic->handle, (void**)&ptr);
	assert(ptr == tic);
	printf("consumer(%d) wakeups:%ld, itemswritten:%ld, items/wakeup:%ld\n",
	    tic->id,
	    tic->n_wakeups, 
	    tic->n_itemswritten, 
	    tic->n_wakeups ? (tic->n_itemswritten/tic->n_wakeups) : 0);
    }

    printf("main ends\n");
    return 0;
}
