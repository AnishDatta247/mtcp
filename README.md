
# Message Oriented TCP (MTCP)

Open 3 terminals first.

In the first terminal, run:
```
make init
./initmsocket
``` 
In the second terminal, run:
```
make user
./user1 127.0.0.1 20000 127.0.0.1 20001
```
In the third terminal, run:
```
./user2 127.0.0.1 20001 127.0.0.1 20000 
```
Now the message will start transferring.

To change the drop probability, change the ```DROP_PROBABILITY``` parameter in ```msocket.h``` and recompile.

To check the messages and ACKs transferred, use Ctrl+C in first terminal to close the initmsocket program with message counts being printed.


