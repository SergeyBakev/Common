//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: RPC_CATEGORY
//
// MessageText:
//
// Rpc Events.
//
#define RPC_CATEGORY                     0x00000001L

//
// MessageId: NT_CATEGORY
//
// MessageText:
//
// NT Events.
//
#define NT_CATEGORY                      0x00000002L

//
// MessageId: USER_CONNECTED
//
// MessageText:
//
// User: %1 connected.
//
#define USER_CONNECTED                   0xC0000003L

//
// MessageId: USER_DISCONNECTED
//
// MessageText:
//
// User: %1 disconnected.
//
#define USER_DISCONNECTED                0xC0000004L

//
// MessageId: NOT_FOUND_LIC_DIRECTORY
//
// MessageText:
//
// Folder:  %1 with licenses not found.
//
#define NOT_FOUND_LIC_DIRECTORY          0xC0000005L

//
// MessageId: RPC_CANNOT_STOPED
//
// MessageText:
//
// RPC server stop unsuccessful 
//
#define RPC_CANNOT_STOPED                0xC0000006L

//
// MessageId: LICENSE_MNGR_DICONNECT_FAIL
//
// MessageText:
//
// An error occurred while disconnecting the license manager
//
#define LICENSE_MNGR_DICONNECT_FAIL      0xC0000007L

//
// MessageId: EVMSG_INSTALLED
//
// MessageText:
//
// The %1 service was installed.
//
#define EVMSG_INSTALLED                  0xC0000064L

//
// MessageId: EVMSG_REMOVED
//
// MessageText:
//
// The %1 service was removed.
//
#define EVMSG_REMOVED                    0xC0000065L

//
// MessageId: EVMSG_NOTREMOVED
//
// MessageText:
//
// The %1 service could not be removed.
//
#define EVMSG_NOTREMOVED                 0xC0000066L

//
// MessageId: EVMSG_CTRLHANDLERNOTINSTALLED
//
// MessageText:
//
// The control handler could not be installed.
//
#define EVMSG_CTRLHANDLERNOTINSTALLED    0xC0000067L

//
// MessageId: EVMSG_FAILEDINIT
//
// MessageText:
//
// The initialization process failed.
//
#define EVMSG_FAILEDINIT                 0xC0000068L

//
// MessageId: EVMSG_STARTED
//
// MessageText:
//
// The service was started.
//
#define EVMSG_STARTED                    0xC0000069L

//
// MessageId: EVMSG_BADREQUEST
//
// MessageText:
//
// The service received an unsupported request.
//
#define EVMSG_BADREQUEST                 0xC000006AL

//
// MessageId: EVMSG_DEBUG
//
// MessageText:
//
// Debug: %1
//
#define EVMSG_DEBUG                      0xC000006BL

//
// MessageId: EVMSG_STOPPED
//
// MessageText:
//
// The service was stopped.
//
#define EVMSG_STOPPED                    0xC000006CL

