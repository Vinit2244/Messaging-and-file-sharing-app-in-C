#ifndef _REQ_CODES_H_
#define _REQ_CODES_H_

// ===================== Request types =====================
#define REQ_UNSERVICED 1
#define ACK 2
#define SIGNIN 3
#define SIGNUP 4
#define SIGNUP_FAILED 5
#define SIGNIN_FAILED 6
#define SIGNOUT 7
#define SIGNOUT_FAILED 8
#define DELETE_ACCOUNT 9
#define DELETE_FAILED 10
#define IMG_DATA 11
#define IMG_DATA_END 12
#define SEND_IMG 13
#define FAIL 14
#define CLIENT_OFFLINE 15
#define INVALID_USER 16
#define INTER_REQ_RECV 17
#define INTER_REQ_SEND 18

// ===================== Status types =====================
#define OFFLINE 0
#define ONLINE 1
#define REQ_RECVD 0

#endif