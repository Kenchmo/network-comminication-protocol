# network-comminication-protocol

Request peer ID

In this request, a peer asks its remote to respond with an (assumedly) unique

ID in the form of a variable length string of characters.



Request Hash

In this request, a peer sends a block of data and one of a set of hash

functions. The peer must calculate the hash of the data, and then respond

with the resultant hash.



A sample output 

Connecting to peer 10.0.10.5...

(Req ID 1): Connected to peer. Requesting ID

(Resp ID 1 received): ID Received “peer 12345”.

(Remote req ID 5): Received request for ID

(Remote resp ID 5): Responded with ID “peer abcde”

(Req ID 2): Requesting SHA256 hash of 1MiB

(Remote req ID 6): Received MD5 request of 1MiB of data from peer...calculating hash

(Resp ID 2 received): Received SHA256 hash 19efd5071ee68721be4fe14......

(Remote resp ID 6): Sent MD5 hash to peer

