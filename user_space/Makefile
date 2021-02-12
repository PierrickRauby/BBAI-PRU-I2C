simple_ping_pong_rpmsg_user_space : simple_ping_pong_rpmsg_user_space.o
	gcc -o simple_ping_pong_rpmsg_user_space simple_ping_pong_rpmsg_user_space.o
	rm -rf *.o
simple_ping_pong_rpmsg_user_space.o : simple_ping_pong_rpmsg_user_space.c
	gcc -o simple_ping_pong_rpmsg_user_space.o -c simple_ping_pong_rpmsg_user_space.c 

clean:
	rm -rf *.o
	rm -rf simple_ping_pong_rpmsg_user_space
#
#
#
#
#
#all: simple_ping_pong_user_space.out \
  #simple_ping_pong_user_space.out
# @echo "built!"
#
#
#clean:
# rm -f *.out
# rm -rf $(gen_dir)
#
