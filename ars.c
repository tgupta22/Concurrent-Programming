// a toy Airline Reservations System

#include "ars.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>

struct flight_info {
  int next_tid; // +1 everytime
  int nr_booked; // booked <= seats

// Defining ticket_lock for the per flight lock.  
  pthread_mutex_t ticket_lock;
   
  //This is being used in the book_flight_can_wait function
  pthread_cond_t wait;
 //  pthread_condattr_t cattr;
   
 //  pthread_cond_t cond; 
  // = PTHREAD_COND_INITIALIZER;
  struct ticket tickets[]; // all issued tickets of this flight
//  pthread_mutext_t ticket_lock;
};

// I was using big_lock before I implemented the per flight lock.

pthread_mutex_t big_lock;
//pthread_mutex_t ticket_lock;

int __nr_flights = 0;
int __nr_seats = 0;
struct flight_info ** flights = NULL;

static int ticket_cmp(const void * p1, const void * p2)
{
  const uint64_t v1 = *(const uint64_t *)p1;
  const uint64_t v2 = *(const uint64_t *)p2;
  if (v1 < v2)
    return -1;
  else if (v1 > v2)
    return 1;
  else
    return 0;
}

void tickets_sort(struct ticket * ts, int n)
{
  qsort(ts, n, sizeof(*ts), ticket_cmp);
}

void ars_init(int nr_flights, int nr_seats_per_flight)
{
  flights = malloc(sizeof(*flights) * nr_flights);
  for (int i = 0; i <= nr_flights; i++) {
    flights[i] = calloc(1, sizeof(flights[i][0]) + (sizeof(struct ticket) * nr_seats_per_flight));
    flights[i]->next_tid = 1;
 
    // Initializing the ticket_lock and cond_wait.
    pthread_mutex_init(&flights[i]->ticket_lock, NULL);

    pthread_cond_init(&flights[i]->wait, NULL);

  
  }
  __nr_flights = nr_flights;
  __nr_seats = nr_seats_per_flight;

  // Initalizing the big_lock here
  pthread_mutex_init(&big_lock, NULL);
}

int book_flight(short user_id, short flight_number)
{

	// Using the per flight lock.
int mutex_lock =  pthread_mutex_lock(&flights[flight_number]->ticket_lock);

if(mutex_lock == 0) {
//	if(pthread_mutex_lock(&big_lock) {
  // wrong number
  if (flight_number >= __nr_flights)
  {
//  pthread_mutex_unlock(&big_lock);
	
pthread_mutex_unlock(&flights[flight_number]->ticket_lock);

	
	  return -1;
    }
  struct flight_info * fi = flights[flight_number];
  // full
  if (fi->nr_booked >= __nr_seats) {
//
//pthread_mutex_unlock(&big_lock);
//

// Unlocking the ticket_lock.	  
  pthread_mutex_unlock(&flights[flight_number]->ticket_lock);

	  return -1;
  }

  int tid = fi->next_tid++;
  // book now
  fi->tickets[fi->nr_booked].uid = user_id;
  fi->tickets[fi->nr_booked].fid = flight_number;
  fi->tickets[fi->nr_booked].tid = tid;
  fi->nr_booked++;
 
  pthread_mutex_unlock(&flights[flight_number]->ticket_lock);
 // pthread_mutex_unlock(&big_lock);
  return tid;
  }

return -1;
	}


// a helper function for cancel/change
// search a ticket of a flight and return its offset if found
static int search_ticket(struct flight_info * fi, short user_id, int ticket_number)
{
  for (int i = 0; i < fi->nr_booked; i++)
    if (fi->tickets[i].uid == user_id && fi->tickets[i].tid == ticket_number)
      return i; // cancelled

  return -1;
}

bool cancel_flight(short user_id, short flight_number, int ticket_number)
{

//pthread_cond_signal(&flights[flight_number]->wait);

if (  pthread_mutex_lock(&flights[flight_number]->ticket_lock) == 0 )

	
//	if(pthread_mutex_lock(&big_lock) == 0)
	{
  if (flight_number >= __nr_flights)
  {

  pthread_mutex_unlock(&flights[flight_number]->ticket_lock);

//	  pthread_mutex_unlock(&big_lock);
    return false;
  }
  struct flight_info * fi = flights[flight_number];
  int offset = search_ticket(fi, user_id, ticket_number);
  if (offset >= 0) {
    fi->tickets[offset] = fi->tickets[fi->nr_booked-1];
    fi->nr_booked--;

    // Calling the cond_signal here for the implementing wait.
    pthread_cond_signal(&flights[flight_number]->wait);


     pthread_mutex_unlock(&flights[flight_number]->ticket_lock);

  //  pthread_mutex_unlock(&big_lock);
    return true; // cancelled
  }


  pthread_mutex_unlock(&flights[flight_number]->ticket_lock);

//  pthread_mutex_unlock(&big_lock);
  return false; // not found
}
}

int change_flight(short user_id, short old_flight_number, int old_ticket_number,
                  short new_flight_number)
{

	 

  // wrong number or no-op
  if (old_flight_number >= __nr_flights ||
      new_flight_number >= __nr_flights ||
      old_flight_number == new_flight_number)
    return -1;

  // two things must be done atomically: (1) cancel the old ticket and (2) book a new ticket
  // if any of the two operations cannot be done, nothing should happen
  // for example, if the new flight has no seat, the existing ticket must remain valid
  // if the old ticket number is invalid, don't acquire a new ticket
  // TODO: your code here

  
  else{
 


        struct flight_info* fi = flights[old_flight_number];


	// Checks for the booked tickets.

	// book_fligt help
  int bk_help = book_flight(user_id, new_flight_number);


          if(bk_help != -1)

        {


		// cancel_flight help to access it.
                bool cl_help = cancel_flight(user_id, old_flight_number, old_ticket_number);

                if(bk_help == false)

                {
                        printf("%s\n", "Ticket Not Found");
                        return -1;
                }

//		pthread_cond_signal(&flights[flight_number]->wait);

                return bk_help;
        }





  else {

	  // Checks for the ticket here.

	 if(search_ticket(fi, user_id, old_ticket_number) == -1){

  //        pthread_mutex_unlock(&big_lock);
  printf("%s\n","Ticket Not found");

          return -1;
  
	 }
	else
	{
	return -1;
	}



  }



}

return -1;

}
/*
  else {

   //       pthread_mutex_unlock(&big_lock);
          printf("Flight Change Fail.\n");

          return -1;
  }




  return -1;
  }
*/
// malloc and dump all tickets in the returned array
struct ticket * dump_tickets(int * n_out)
{
  int n = 0;
  for (int i = 0; i < __nr_flights; i++)
    n += flights[i]->nr_booked;

  struct ticket * const buf = malloc(sizeof(*buf) * n);
  assert(buf);
  n = 0;
  for (int i = 0; i < __nr_flights; i++) {
    memcpy(buf+n, flights[i]->tickets, sizeof(*buf) * flights[i]->nr_booked);
    n += flights[i]->nr_booked;
  }
  *n_out = n; // number of tickets
  return buf;
}

int book_flight_can_wait(short user_id, short flight_number)
{

	 // wrong number
  if (flight_number >= __nr_flights)
    return -1;

  
	 pthread_mutex_lock(&flights[flight_number]->ticket_lock);

//	     pthread_cond_wait(&flights[i]->wait, ticket_lock);

	 // Making a while loop so that implement and check for the cond_wait.

	     while(flights[flight_number]->nr_booked  > __nr_seats ) {

		     // cond_wait would be having 2 fields as it also accounts for wait and per flight lock, TA guided here.
  pthread_cond_wait(&flights[flight_number]->wait,&flights[flight_number]-> ticket_lock);

  
  // pthread_mutex_unlock(&flights[flight_number]->ticket_lock);
	     }
  // return book_flight(user_id, flight_number);

	     // Using the below from the book_flight function.

	      struct flight_info * fi = flights[flight_number];


     int tid =fi->next_tid++;
  // book now
  fi->tickets[fi->nr_booked].uid = user_id;
  fi->tickets[fi->nr_booked].fid = flight_number;
  fi->tickets[fi->nr_booked].tid = tid;
  fi->nr_booked++;

pthread_mutex_unlock(&flights[flight_number]->ticket_lock);

return tid; 
}
