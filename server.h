/* 
 * matchmaking server.
 * allows clients to see who is on the server and start a game with people or something like that
 * registers game with the register_game function of the chess.h file
 *
 */

#ifndef __SERVER_H
#define __SERVER_H


int start_server_ipv4(int port);

int start_server_ipv6(int port);

#endif 
