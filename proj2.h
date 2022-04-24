/*
 * File: proj2.h
 * Author: Pedro Lameiras ist199540
 * Description: Header file containing constants, function headers and
 * structures used throughout the second project.
*/

#ifndef _PROJ2_H_
#define _PROJ2_H_


#define BUFF 65535
#define OPTION_CHAR 0
#define DATE_FIELDS 3
#define DAY 0
#define MONTH 1
#define YEAR 2
#define TIME_FIELDS 2
#define HOUR 0
#define MINUTE 1
#define MAX_NUMBER_AIRPORTS 40
#define AIRPORT_ID_MAX_SIZE 4
#define AIRPORT_COUNTRY_NAME_MAX_SIZE 31
#define AIRPORT_CITY_NAME_MAX_SIZE 51
#define MAX_NUMBER_FLIGHTS 30000
#define FLIGHT_CODE_MAX_SIZE 7
#define RESERVATION_CODE_MAX_SIZE 65530

#define ADD_AIRPORT_ERRORS 3
#define INVALID_AIRPORT_ID_ERROR 0
#define INVALID_AIRPORT_ID_ERROR_MESSAGE "invalid airport ID\n"
#define TOO_MANY_AIRPORTS_ERROR 1
#define TOO_MANY_AIRPORTS_ERROR_MESSAGE "too many airports\n"
#define DUPLICATE_AIRPORT_ERROR 2
#define DUPLICATE_AIRPORT_ERROR_MESSAGE "duplicate airport\n"

#define NO_SUCH_AIRPORT_ID_ERROR_MESSAGE "%s: no such airport ID\n"

#define ADD_OR_LIST_FLIGHTS_ERRORS 8
#define INVALID_FLIGHT_CODE_ERROR 0
#define INVALID_FLIGHT_CODE_ERROR_MESSAGE "invalid flight code\n"
#define FLIGHT_ALREADY_EXISTS_ERROR 1
#define FLIGHT_ALREADY_EXISTS_ERROR_MESSAGE "flight already exists\n"
#define NO_SUCH_AIRPORT_ID_1_ERROR 2
#define NO_SUCH_AIRPORT_ID_1_ERROR_MESSAGE "%s: no such airport ID\n"
#define NO_SUCH_AIRPORT_ID_2_ERROR 3
#define NO_SUCH_AIRPORT_ID_2_ERROR_MESSAGE "%s: no such airport ID\n"
#define TOO_MANY_FLIGHTS_ERROR 4
#define TOO_MANY_FLIGHTS_ERROR_MESSAGE "too many flights\n"
#define INVALID_DATE_ERROR 5
#define INVALID_DATE_ERROR_MESSAGE "invalid date\n"
#define INVALID_DURATION_ERROR 6
#define INVALID_DURATION_ERROR_MESSAGE "invalid duration\n"
#define INVALID_CAPACITY_ERROR 7
#define INVALID_CAPACITY_ERROR_MESSAGE "invalid capacity\n"

#define INVALID_DATE_ERROR_MESSAGE "invalid date\n"

#define NO_MEMORY_ERROR_MESSAGE "No memory.\n"

#define INVALID_RESERVATION_CODE_ERROR_MESSAGE "invalid reservation code\n"
#define NON_EXISTENT_FLIGHT_ERROR_MESSAGE "%s: flight does not exist\n"
#define ALREADY_EXISTENT_RESERVATION_CODE "%s: flight reservation already used\n"
#define TOO_MANY_RESERVATIONS_ERROR_MESSAGE "too many reservations\n"
#define INVALID_PASSENGER_NUMBER_ERROR_MESSAGE "invalid passenger number\n"

#define NON_EXISTENT_CODE_ERROR_MESSAGE "not found\n"

#define FLIGHT_HOUR_MAX_DURATION 12
#define FLIGHT_MINUTE_MAX_DURATION 0

#define FLIGHT_CAPACITY_LOWER_BOUND 10

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

#define MINUTES_IN_HOUR 60
#define HOURS_IN_DAY 24


typedef struct reservation {
    char reservationCode[RESERVATION_CODE_MAX_SIZE];
    int numberOfPassengers;
    struct reservation *next;
} reservation;

typedef struct airport {
    char ID[AIRPORT_ID_MAX_SIZE];
    char country[AIRPORT_COUNTRY_NAME_MAX_SIZE];
    char city[AIRPORT_CITY_NAME_MAX_SIZE];
    int departureFlights;
} airport;

typedef struct flight {
    char departureAirport[AIRPORT_ID_MAX_SIZE];
    char arrivalAirport[AIRPORT_ID_MAX_SIZE];
    int departureDate[DATE_FIELDS];
    int departureTime[TIME_FIELDS];
    int duration[TIME_FIELDS];
    int capacity;
    int numberOfPassengers;
    int arrivalDate[DATE_FIELDS];
    int arrivalTime[TIME_FIELDS];
    char flightCode[FLIGHT_CODE_MAX_SIZE];
    struct flight *next;
    struct reservation *reservations;
} flight;

/* Main functions. */

void AddAirport(airport airportList[MAX_NUMBER_AIRPORTS], int *numberOfAirports,
               char input[BUFF]);

void listAirports(airport airportList[MAX_NUMBER_AIRPORTS],
                  int numberOfAirports, char input[BUFF]);

flight *addOrListFlights(airport airportList[MAX_NUMBER_AIRPORTS],
                     flight *flightList, int *numberOfFlights,
                     int currentDate[DATE_FIELDS], char input[BUFF]);

void listAirportDepartureFlights(airport airportList[MAX_NUMBER_AIRPORTS],
                                 flight *flightList,
                                 char input[BUFF]);

void listAirportArrivalFlights(airport airportList[MAX_NUMBER_AIRPORTS],
                               flight *flightList,
                               char input[BUFF]);

void advanceDate(int date[DATE_FIELDS], char input[BUFF]);

void addReservationOrListFlightReservations(flight *flightList,
                                            int currentDate[DATE_FIELDS],
                                            char input[BUFF]);

flight *eliminateFlightOrReservation(flight *flightList, int *numberOfFlights,
                                     char input[BUFF]);

/* Auxiliary functions. */

void createAirport(airport airportList[MAX_NUMBER_AIRPORTS],
                   char ID[AIRPORT_ID_MAX_SIZE],
                   char country[AIRPORT_COUNTRY_NAME_MAX_SIZE],
                   char city[AIRPORT_CITY_NAME_MAX_SIZE],
                   int numberOfAirports);

void printAirport(airport airport);

int airportIDAlphabeticalOrder(char ID1[AIRPORT_ID_MAX_SIZE],
                               char ID2[AIRPORT_ID_MAX_SIZE]);

flight *createFlight(flight *flightList,
                  char departureAirport[AIRPORT_ID_MAX_SIZE],
                  char arrivalAirport[AIRPORT_ID_MAX_SIZE],
                  int departureDate[DATE_FIELDS],
                  int departureTime[TIME_FIELDS], int duration[TIME_FIELDS],
                  int capacity, char flightCode[FLIGHT_CODE_MAX_SIZE]);

void printFlight(flight *flight);

void addOrListFlightsErrors(int errors[ADD_OR_LIST_FLIGHTS_ERRORS],
                            char departureAirport[AIRPORT_ID_MAX_SIZE],
                            char arrivalAirport[AIRPORT_ID_MAX_SIZE],
                            char flightCode[FLIGHT_CODE_MAX_SIZE],
                            int departureDate[DATE_FIELDS],
                            int duration[TIME_FIELDS], int capacity,
                            int currentDate[DATE_FIELDS],
                            airport airportList[MAX_NUMBER_AIRPORTS],
                            flight *flightList,
                            int numberOfFlights);

int checkDateDifference(int date1[DATE_FIELDS], int date2[DATE_FIELDS]);

void printDepartureFlight(flight *departureFlight);

void printArrivalFlight(flight *arrivalFlight);

int compareDateAndTime(int date1[DATE_FIELDS], int time1[TIME_FIELDS],
                       int date2[DATE_FIELDS], int time2[TIME_FIELDS]);

void calculateArrivalDateAndTime(flight *flight);

void setFlightArrivalTime(flight *flight, int hour, int minute);

void incrementFlightArrivalDate(flight *flight);

void setFlightArrivalDate(flight *flight, int day, int month, int year);

void setDate(int oldDate[DATE_FIELDS], int newDate[DATE_FIELDS]);

void printDate(int date[DATE_FIELDS]);


void printAllFlights(flight *flightList);

void freeAllMemory(flight *flightList);

void freeAllReservationsMemory(flight *flight);

/* Auxiliary functions related to the -r and -e commands. */

flight *getFlight(flight *flightList, char flightCode[FLIGHT_CODE_MAX_SIZE],
                  int date[DATE_FIELDS]);

int checkReservationCodeValidity(char *reservationCode);

int checkReservationCodeExistence(flight *flightList,
                                  char reservationCode
                                  [RESERVATION_CODE_MAX_SIZE]);

void addReservationToFlight(flight *flight,
                            char reservationCode[RESERVATION_CODE_MAX_SIZE],
                            int numberOfPassengers);

void printAllFlightReservations(flight *flight);

int checkFlightExistence(flight *flightList,
                         char flightCode[FLIGHT_CODE_MAX_SIZE]);

void eliminateReservation(flight *flightList,
                          char reservationCode[RESERVATION_CODE_MAX_SIZE]);

flight *eliminateFlight(flight *flightList, int *numberOfFlights,
                        char flightCode[FLIGHT_CODE_MAX_SIZE]);

#endif
