#pragma once

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#define bcmp(b1,b2,len) (memcmp(b1,b2,len))
#define snprintf _snprintf
#define strtok_r strtok_s

#define __func__ __FUNCTION__
