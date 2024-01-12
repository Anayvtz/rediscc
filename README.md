# rediscc
implementing shallow version of Redis server and client.
Redis is an in memory data base for data structures.
I have implemented redis for key and value with possibility of other data structure.

to compile the code:
$make all
or to compile only the server:
$make rediscc_srvr 
to compile only the client:
$make rediscc_clnt

client and server are supporting the following commands:
PING - on which client recieves PONG
SET <key> <value> - to add key string to the in memory database
SETEX <key> <expire-seconds> <value> - to add string which will expire and deleted from the in memory data base in *expire-seconds* seconds.
GET <key> - to retrieve a value from the in memory database.
