all: PHK
PHK:chacha20.o curve25519.o ed25519.o poly1305.o rfc6234-master/usha.o rfc6234-master/sha1.o rfc6234-master/sha224-256.o rfc6234-master/sha384-512.o rfc6234-master/hkdf.o rfc6234-master/hmac.o main.o PHKAccessory.o PHKControllerRecord.o PHKNetworkIP.o srp/srp.o srp/cstr.o srp/t_math.o srp/srp6_server.o srp/t_conf.o srp/t_conv.o srp/t_pw.o srp/t_misc.o srp/t_truerand.o srp/t_read.o
	g++ -lcrypto -ldl -o PHK $?
chacha20.o: Chacha20/chacha20_simple.c Chacha20/chacha20_simple.h
	gcc -w -o chacha20.o -c Chacha20/chacha20_simple.c
curve25519.o: curve25519/curve25519-donna.c curve25519/curve25519-donna.h
	gcc -w -o curve25519.o -c curve25519/curve25519-donna.c
ed25519.o: ed25519-donna/ed25519.c ed25519-donna/ed25519.h
	gcc -w -o ed25519.o -c ed25519-donna/ed25519.c
poly1305.o: poly1305-opt-master/poly1305.c poly1305-opt-master/poly1305.h
	gcc -w -o poly1305.o -c poly1305-opt-master/poly1305.c
rfc6234-master/%.o: rfc6234-master/%.c
	gcc -w -c $< -o $@
srp/%.o: srp/%.c
	gcc -lcrypto -lssl -w -c $< -o $@
%.o: %.cpp
	g++ -w -ldl -c $<
clean:
	rm -rf ./*.o Chacha20/*.o curve25519/*.o ed25519-donna/*.o poly1305-opt-master/*.o rfc6234-master/*.o srp/*.o PHK
