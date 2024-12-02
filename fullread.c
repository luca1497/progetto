#include "fullops.h"


ssize_t FullRead(int fd, void *buf, size_t count)  
{ 
    size_t nleft; 
    ssize_t nread; 
    nleft = count; 
    while (nleft > 0) {             /* repeat until no left */
      if ( (nread = read(fd, buf, nleft)) < 0) { 
          if (errno == EINTR) {   /* if interrupted by system call */
                continue;           /* repeat the loop */  
            } else { 
                exit(nread);      /* otherwise exit */  
          }
        } else if (nread == 0) {    /* EOF */  
            break;                  /* break loop here */  
        }   
        nleft -= nread;             /* set left to read */  
        buf += nread;                /* set pointer */  
    }
	  buf = 0; 
    return (nleft); 
}
