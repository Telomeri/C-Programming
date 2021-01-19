
CFLAGS=-Wall -pedantic

all: bank_server client
	

bank_server: desk.o bank_commands.o start_bank.o
	gcc -o bank_connect desk.o bank_commands.o start_bank.o -lpthread -I.

client: client.o  packet_input.o
	gcc -o client_connect client.o packet_input.o -I.

clean:
	rm -rf *.o *~ *.log *bank_connect *client_connect

clean_all: 
	rm -rf *.o *~ *.log *bank_connect *client_connect *bank_accounts

