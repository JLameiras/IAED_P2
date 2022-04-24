/*
 * File: proj2.c
 * Author: Pedro Lameiras ist199540
 * Description: Second IAED project - Adds reservations and the possibility
 * of deleting flights and reservations to the first project.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj2.h"

/* Program's main function. Creates a list of airports and flights and
 * sets the system's date. Continuously waits for a new command until
 * termination. */
int main() {

    char input[BUFF];
    int date[DATE_FIELDS] = {1, 1, 2022};
    airport airportList[MAX_NUMBER_AIRPORTS] = {0};
    flight *flightList = NULL;
    int numberOfAirports = 0;
    int numberOfFlights = 0;

    while(1) {
        fgets(input, BUFF, stdin);
        switch (input[OPTION_CHAR]) {
            case 'a':
                AddAirport(airportList, &numberOfAirports, input);
                break;
            case 'l':
                listAirports(airportList, numberOfAirports, input);
                break;
            case 'v':
                if(flightList == NULL)
                    flightList = addOrListFlights(airportList, flightList,
                                              &numberOfFlights, date, input);
                else
                    addOrListFlights(airportList, flightList,
                                     &numberOfFlights, date, input);
                break;
            case 'p':
                listAirportDepartureFlights(airportList, flightList, input);
                break;
            case 'c':
                listAirportArrivalFlights(airportList, flightList, input);
                break;
            case 't':
                advanceDate(date, input);
                break;
            case 'r':
                addReservationOrListFlightReservations(flightList, date, input);
                break;
            case 'e':
                flightList = eliminateFlightOrReservation(flightList,
                                                          &numberOfFlights,
                                                          input);
                break;
            case 'q':
                freeAllMemory(flightList);
                return 0;
        }
    }
}

/* Main functions. */

/* Handles the 'a' command.
 * Adds an airport to the system's list of airports. */
void AddAirport(airport airportList[MAX_NUMBER_AIRPORTS], int *numberOfAirports,
               char input[BUFF]) {

    int errors[ADD_AIRPORT_ERRORS] = {0, 0, 0};
    char ID[AIRPORT_ID_MAX_SIZE];
    char country[AIRPORT_COUNTRY_NAME_MAX_SIZE];
    char city[AIRPORT_CITY_NAME_MAX_SIZE];
    int i;

    /* A city may contain empty spaces. */
    sscanf(input, "a %s %s %[^\n]", ID, country, city);

    for(i = 0; i < ADD_AIRPORT_ERRORS; i++)
        if(ID[i] < 'A' || ID[i] > 'Z')
            errors[INVALID_AIRPORT_ID_ERROR] = 1;

    if(*numberOfAirports == MAX_NUMBER_AIRPORTS)
        errors[TOO_MANY_AIRPORTS_ERROR] = 1;

    for(i = 0; i < *numberOfAirports; i++)
        if(strcmp(airportList[i].ID, ID) == 0)
            errors[DUPLICATE_AIRPORT_ERROR] = 1;

    if(errors[INVALID_AIRPORT_ID_ERROR]){
        printf(INVALID_AIRPORT_ID_ERROR_MESSAGE);
        return;
    }
    if(errors[TOO_MANY_AIRPORTS_ERROR]) {
        printf(TOO_MANY_AIRPORTS_ERROR_MESSAGE);
        return;
    }
    if(errors[DUPLICATE_AIRPORT_ERROR]) {
        printf(DUPLICATE_AIRPORT_ERROR_MESSAGE);
        return;
    }

    createAirport(airportList, ID, country, city, *numberOfAirports);

    (*numberOfAirports)++;
}

/* Handles the 'l' command.
 * Lists either one, multiple or all airports stored in the system. */
void listAirports(airport airportList[MAX_NUMBER_AIRPORTS],
                  int numberOfAirports, char input[BUFF]){

    int noSuchAirportError, listAllAirports, i;
    char airports[BUFF], *airportIterator;

    listAllAirports = sscanf(input, "l %[^\\n]", airports);

    airportIterator = strtok(airports, " \n");

    if((airportIterator != NULL) && (listAllAirports != -1)) {
        /* Repeatedly gets the next airport ID in the line and prints it. */
        while (airportIterator != NULL) {
            noSuchAirportError = 1;
            for(i = 0; i < MAX_NUMBER_AIRPORTS; i++){
                if(strncmp(airportList[i].ID, airportIterator,
                           AIRPORT_ID_MAX_SIZE - 1) == 0){
                    noSuchAirportError = 0;
                    printAirport(airportList[i]);
                    break;
                }
            }
            if(noSuchAirportError)
                printf(NO_SUCH_AIRPORT_ID_ERROR_MESSAGE, airportIterator);
            airportIterator = strtok(NULL, " \n");
        }
    } else {
        /* Lists all airports by alphabetical order by creating a vector of
         * indexes and sorting it. */
        int indexAux, flag = 1;
        int allAirports[MAX_NUMBER_AIRPORTS] = {0};
        for(i = 0; i < numberOfAirports; i++)
            allAirports[i] = i;

        /* BubbleSort. Goes repeatedly over the vector of indexes until no
         * remaining unordered pair is found. */
        while(flag) {
            flag = 0;
            for (i = 0; i < (numberOfAirports - 1); i++) {
                if(airportIDAlphabeticalOrder(airportList[allAirports[i]].ID,
                                              airportList[allAirports[i+1]].ID)){
                    indexAux = allAirports[i];
                    allAirports[i] = allAirports[i+1];
                    allAirports[i+1] = indexAux;
                    flag = 1;
                }
            }
        }

        for(i = 0; i < numberOfAirports; i++)
            printAirport(airportList[allAirports[i]]);
    }
}

/* Handles the 'v' command.
 * Adds a flight to the flight list of a specified airport or lists all
 * flights related to a given airport. */
flight *addOrListFlights(airport airportList[MAX_NUMBER_AIRPORTS],
                     flight *flightList, int *numberOfFlights,
                     int currentDate[DATE_FIELDS], char input[BUFF]){

    int errors[ADD_OR_LIST_FLIGHTS_ERRORS] = {0, 0, 1, 1, 0, 0, 0, 0};
    char departureAirport[AIRPORT_ID_MAX_SIZE],
         arrivalAirport[AIRPORT_ID_MAX_SIZE],
         flightCode[FLIGHT_CODE_MAX_SIZE];
    int departureDate[DATE_FIELDS], departureTime[TIME_FIELDS],
            duration[TIME_FIELDS], capacity, option, i;

    option = sscanf(input, "v %s %s %s %d-%d-%d %d:%d %d:%d %d", flightCode,
                    departureAirport, arrivalAirport, &departureDate[DAY],
                    &departureDate[MONTH], &departureDate[YEAR],
                    &departureTime[HOUR], &departureTime[MINUTE],
                    &duration[HOUR], &duration[MINUTE], &capacity);

    /* If only the command 'v' was inserted prints all flights in the system. */
    if(option == -1){
        printAllFlights(flightList);
        return flightList;
    }

    /* Handles all errors related to the flight. */
    addOrListFlightsErrors(errors, departureAirport, arrivalAirport, flightCode,
                           departureDate, duration, capacity, currentDate,
                           airportList, flightList, *numberOfFlights);

    if(errors[INVALID_FLIGHT_CODE_ERROR]) {
        printf(INVALID_FLIGHT_CODE_ERROR_MESSAGE);
        return NULL;
    }
    if(errors[FLIGHT_ALREADY_EXISTS_ERROR]) {
        printf(FLIGHT_ALREADY_EXISTS_ERROR_MESSAGE);
        return NULL;
    }
    if(errors[NO_SUCH_AIRPORT_ID_1_ERROR]) {
        printf(NO_SUCH_AIRPORT_ID_1_ERROR_MESSAGE, departureAirport);
        return NULL;
    }
    if(errors[NO_SUCH_AIRPORT_ID_2_ERROR]) {
        printf(NO_SUCH_AIRPORT_ID_2_ERROR_MESSAGE, arrivalAirport);
        return NULL;
    }
    if(errors[TOO_MANY_FLIGHTS_ERROR]) {
        printf(TOO_MANY_FLIGHTS_ERROR_MESSAGE);
        return NULL;
    }
    if(errors[INVALID_DATE_ERROR]) {
        printf(INVALID_DATE_ERROR_MESSAGE);
        return NULL;
    }
    if(errors[INVALID_DURATION_ERROR]) {
        printf(INVALID_DURATION_ERROR_MESSAGE);
        return NULL;
    }
    if(errors[INVALID_CAPACITY_ERROR]) {
        printf(INVALID_CAPACITY_ERROR_MESSAGE);
        return NULL;
    }

    flightList = createFlight(flightList, departureAirport, arrivalAirport,
                 departureDate, departureTime, duration, capacity, flightCode);

    /* Increments the number of departure flights of the airport of which the
     * newly added flight stems from. */
    for(i = 0; i < MAX_NUMBER_AIRPORTS; i++)
        if(strcmp(airportList[i].ID, departureAirport) == 0)
            airportList[i].departureFlights++;

    (*numberOfFlights)++;

    return flightList;
}

/* Handles the 'p' command.
 * Lists all flights which have departed from a certain airport stored. */
void listAirportDepartureFlights(airport airportList[MAX_NUMBER_AIRPORTS],
                                 flight *flightList,
                                 char input[BUFF]){

    int noSuchAirportIDError = 1;
    flight *airportFlights[MAX_NUMBER_FLIGHTS];
    char ID[AIRPORT_ID_MAX_SIZE];
    int i, j = 0, flag = 1;
    flight *aux, *indexAux;

    sscanf(input, "p %s", ID);

    for(i = 0; i < MAX_NUMBER_AIRPORTS; i++)
        if(strcmp(airportList[i].ID, ID) == 0)
            noSuchAirportIDError = 0;

    if(noSuchAirportIDError){
        printf(NO_SUCH_AIRPORT_ID_1_ERROR_MESSAGE, ID);
        return;
    }

    for(aux = flightList; aux != NULL; aux = aux->next)
        if(strcmp(aux->departureAirport, ID) == 0)
            airportFlights[j++] = aux;

    /* BubbleSort. */
    while(flag) {
        flag = 0;
        for (i = 0; i < (j - 1); i++) {
            if(compareDateAndTime(airportFlights[i]->departureDate,
                                  airportFlights[i]->departureTime,
                                  airportFlights[i+1]->departureDate,
                                  airportFlights[i+1]->departureTime)){
                indexAux = airportFlights[i];
                airportFlights[i] = airportFlights[i+1];
                airportFlights[i+1] = indexAux;
                flag = 1;
            }
        }
    }

    for(i = 0; i < j; i++)
        printDepartureFlight(airportFlights[i]);
}

/* Handles the 'c' command.
 * Lists all flights which have arrived in a certain airport stored. */
void listAirportArrivalFlights(airport airportList[MAX_NUMBER_AIRPORTS],
                               flight *flightList,
                               char input[BUFF]){

    int noSuchAirportIDError = 1;
    flight *airportFlights[MAX_NUMBER_FLIGHTS];
    char ID[AIRPORT_ID_MAX_SIZE];
    int i, j = 0, flag = 1;
    flight *aux, *indexAux;

    sscanf(input, "c %s", ID);

    for(i = 0; i < MAX_NUMBER_AIRPORTS; i++)
        if(strcmp(airportList[i].ID, ID) == 0)
            noSuchAirportIDError = 0;

    if(noSuchAirportIDError){
        printf(NO_SUCH_AIRPORT_ID_1_ERROR_MESSAGE, ID);
        return;
    }

    for(aux = flightList; aux != NULL; aux = aux->next)
        if(strcmp(aux->arrivalAirport, ID) == 0)
            airportFlights[j++] = aux;

    /* Bubble sorts the vector by chronological order. */
    while(flag) {
        flag = 0;
        for (i = 0; i < (j - 1); i++) {
            if (compareDateAndTime(airportFlights[i]->arrivalDate,
                                   airportFlights[i]->arrivalTime,
                                   airportFlights[i+1]->arrivalDate,
                                   airportFlights[i+1]->arrivalTime)) {
                indexAux = airportFlights[i];
                airportFlights[i] = airportFlights[i + 1];
                airportFlights[i + 1] = indexAux;
                flag = 1;
            }
        }
    }

    for(i = 0; i < j; i++)
        printArrivalFlight(airportFlights[i]);
}

/* Handles the 't' command.
 * Advances the system date assuming the given date conforms to the set
 * parameters. */
void advanceDate(int date[DATE_FIELDS], char input[BUFF]){
    int newDate[DATE_FIELDS];

    sscanf(input, "t %d-%d-%d", &newDate[DAY], &newDate[MONTH], &newDate[YEAR]);

    /* Discards date change if parameters are off. */
    if(!checkDateDifference(date, newDate)){
        printf(INVALID_DATE_ERROR_MESSAGE);
        return;
    }

    setDate(date, newDate);

    printDate(date);
}

/* Handles the 'r' command.
 * Either adds a reservation or lists all the reservations related to an
 * existing flight. */
void addReservationOrListFlightReservations(flight *flightList,
                                            int currentDate[DATE_FIELDS],
                                            char input[BUFF]){

    int listAllReservations;
    char flightCode[FLIGHT_CODE_MAX_SIZE];
    int date[DATE_FIELDS];
    /* Continuously broken down by strtok to get all the reservations. */
    char reservations[BUFF];
    /* Holds each new reservation extracted from reservations. */
    char *reservationIterator;

    flight *flight;
    char reservationCode[RESERVATION_CODE_MAX_SIZE];
    int numberOfPassengers;

    /* If the sscanf reads more than 4 elements, then it means there are one or
     * more reservations to be added to the input flight.*/
    listAllReservations = sscanf(input, "r %s %d-%d-%d %[^\\n]", flightCode,
                                 &date[DAY], &date[MONTH], &date[YEAR],
                                 reservations);

    reservationIterator = strtok(reservations, " \n");

    /* Adds reservation to flight. */
    if((reservationIterator != NULL) && (listAllReservations > 4)) {
        while (reservationIterator != NULL) {
            /* Manages the reservation code of the reservations. */
            strcpy(reservationCode, reservationIterator);

            if(checkReservationCodeValidity(reservationIterator)){
                printf(INVALID_RESERVATION_CODE_ERROR_MESSAGE);
                return;
            }

            flight = getFlight(flightList, flightCode, date);
            if(flight == NULL){
                printf(NON_EXISTENT_FLIGHT_ERROR_MESSAGE, flightCode);
                return;
            }

            if(checkReservationCodeExistence(flightList, reservationCode)){
                printf(ALREADY_EXISTENT_RESERVATION_CODE, reservationIterator);
                return;
            }

            /* Manages the number of passengers of the reservation. */
            reservationIterator = strtok(NULL, " \n");

            numberOfPassengers = atoi(reservationIterator);

            if(flight->capacity <
               (flight->numberOfPassengers + numberOfPassengers)){
                printf(TOO_MANY_RESERVATIONS_ERROR_MESSAGE);
                return;
            }

            if(checkDateDifference(date, currentDate)){
                printf(INVALID_DATE_ERROR_MESSAGE);
                return;
            }

            if(numberOfPassengers < 1){
                printf(INVALID_PASSENGER_NUMBER_ERROR_MESSAGE);
                return;
            }

            addReservationToFlight(flight, reservationCode,
                                   numberOfPassengers);
            flight->numberOfPassengers += numberOfPassengers;

            reservationIterator = strtok(NULL, " \n");

        }
    } else {
        /* Lists all reservations of the flight. */
        flight = getFlight(flightList, flightCode, date);
        if(flight == NULL){
            printf(NON_EXISTENT_FLIGHT_ERROR_MESSAGE, flightCode);
            return;
        }

        if(checkDateDifference(date, currentDate)){
            printf(INVALID_DATE_ERROR_MESSAGE);
            return;
        }

        printAllFlightReservations(flight);
    }
}

/* Handles the 'e' command.
 * Either eliminates one or more flights or a reservation. */
flight *eliminateFlightOrReservation(flight *flightList,
                                     int *numberOfFlights,
                                     char input[BUFF]){
    char code[RESERVATION_CODE_MAX_SIZE];
    char flightCode[FLIGHT_CODE_MAX_SIZE];

    sscanf(input, "e %s", code);

    /* In this case we're dealing with a flight. */
    if(strlen(code) < 10){
        strncpy(flightCode, code, FLIGHT_CODE_MAX_SIZE);

        if(!checkFlightExistence(flightList, flightCode)) {
            printf(NON_EXISTENT_CODE_ERROR_MESSAGE);
            return flightList;
        }

        flightList = eliminateFlight(flightList, numberOfFlights, flightCode);

        return flightList;
    }

    /* In this case, we're dealing with a reservation. */
    if(!checkReservationCodeExistence(flightList, code)) {
        printf(NON_EXISTENT_CODE_ERROR_MESSAGE);
        return flightList;
    }

    eliminateReservation(flightList, code);

    return flightList;
}


/* Auxiliary functions. */

/* Creates an airport by copying its data onto the next free index in
 * the airport list. */
void createAirport(airport airportList[MAX_NUMBER_AIRPORTS],
                   char ID[AIRPORT_ID_MAX_SIZE],
                   char country[AIRPORT_COUNTRY_NAME_MAX_SIZE],
                   char city[AIRPORT_CITY_NAME_MAX_SIZE], int numberOfAirports){

    strcpy(airportList[numberOfAirports].ID, ID);
    strcpy(airportList[numberOfAirports].country, country);
    strcpy(airportList[numberOfAirports].city, city);

    /* Prints freshly created airport. */
    printf("airport %s\n", ID);
}

/* Prints a given airport. */
void printAirport(airport airport){

    printf("%s %s %s %d\n", airport.ID, airport.city, airport.country,
           airport.departureFlights);
}

/* Used to sort airports by their alphabetical order.
 * Returns 0 if the airport ID1 comes alphabetically before the ID2, 1
 * otherwise. */
int airportIDAlphabeticalOrder(char ID1[AIRPORT_ID_MAX_SIZE],
                               char ID2[AIRPORT_ID_MAX_SIZE]){

    if(strcmp(ID1, ID2) > 0)
        return 1;

    return 0;
}

/* Creates a flight by copying its data onto the next free index in
 * the flight list */
flight *createFlight(flight *flightList,
                  char departureAirport[AIRPORT_ID_MAX_SIZE],
                  char arrivalAirport[AIRPORT_ID_MAX_SIZE],
                  int departureDate[DATE_FIELDS],
                  int departureTime[DATE_FIELDS], int duration[TIME_FIELDS],
                  int capacity, char flightCode[FLIGHT_CODE_MAX_SIZE]){
    flight *newFlight = (flight*)calloc(1, sizeof(flight));
    flight *aux;

    /* Not enough memory. */
    if (newFlight == NULL) {
        printf(NO_MEMORY_ERROR_MESSAGE);
        freeAllMemory(flightList);
        exit(EXIT_FAILURE);
    }

    strcpy(newFlight->departureAirport, departureAirport);
    strcpy(newFlight->arrivalAirport, arrivalAirport);
    newFlight->departureDate[DAY] = departureDate[DAY];
    newFlight->departureDate[MONTH] = departureDate[MONTH];
    newFlight->departureDate[YEAR] = departureDate[YEAR];
    newFlight->departureTime[HOUR] = departureTime[HOUR];
    newFlight->departureTime[MINUTE] = departureTime[MINUTE];
    newFlight->duration[HOUR] = duration[HOUR];
    newFlight->duration[MINUTE] = duration[MINUTE];
    newFlight->capacity = capacity;
    newFlight->numberOfPassengers = 0;
    strcpy(newFlight->flightCode, flightCode);
    calculateArrivalDateAndTime(newFlight);

    newFlight->next = NULL;
    newFlight->reservations = NULL;

    if(flightList == NULL) {
        return newFlight;
    } else {
        for(aux = flightList; aux->next != NULL; aux = aux->next);
        aux->next = newFlight;
    }

    return flightList;
}

/* Prints a given flight. */
void printFlight(flight *flight){
    printf("%s %s %s %02d-%02d-%d %02d:%02d\n", flight->flightCode,
           flight->departureAirport, flight->arrivalAirport,
           flight->departureDate[DAY], flight->departureDate[MONTH],
           flight->departureDate[YEAR], flight->departureTime[HOUR],
           flight->departureTime[MINUTE]);
}

/* Handles all possible errors regarding the addOrListFlight function.*/
void addOrListFlightsErrors(int errors[ADD_OR_LIST_FLIGHTS_ERRORS],
                            char departureAirport[AIRPORT_ID_MAX_SIZE],
                            char arrivalAirport[AIRPORT_ID_MAX_SIZE],
                            char flightCode[FLIGHT_CODE_MAX_SIZE],
                            int departureDate[DATE_FIELDS],
                            int duration[TIME_FIELDS], int capacity,
                            int currentDate[TIME_FIELDS],
                            airport airportList[MAX_NUMBER_AIRPORTS],
                            flight *flightList,
                            int numberOfFlights){

    int i;
    flight *aux;

    if((flightCode[0] < 'A' || flightCode[0] > 'Z') ||
       (flightCode[1] < 'A' || flightCode[1] > 'Z'))
        errors[INVALID_FLIGHT_CODE_ERROR] = 1;

    if((flightCode[2] == '0') || (flightCode[2] == 0))
        errors[INVALID_FLIGHT_CODE_ERROR] = 1;

    for(i = 2; i < 6; i++)
        if(flightCode[i] < '0' || flightCode[i] > '9'){
            if(flightCode[i] == 0)
                break;
            errors[INVALID_FLIGHT_CODE_ERROR] = 1;
        }

    for(aux = flightList; aux != NULL; aux = aux->next){
        if(strcmp(flightCode, aux->flightCode) == 0)
            if(departureDate[DAY] == aux->departureDate[DAY])
                if(departureDate[MONTH] == aux->departureDate[MONTH])
                    if(departureDate[YEAR] == aux->departureDate[YEAR])
                        errors[FLIGHT_ALREADY_EXISTS_ERROR] = 1;
    }

    for(i = 0; i < MAX_NUMBER_AIRPORTS; i++){
        if(strcmp(airportList[i].ID, departureAirport) == 0)
            errors[NO_SUCH_AIRPORT_ID_1_ERROR] = 0;
        if(strcmp(airportList[i].ID, arrivalAirport) == 0)
            errors[NO_SUCH_AIRPORT_ID_2_ERROR] = 0;
    }

    if(numberOfFlights == MAX_NUMBER_FLIGHTS)
        errors[TOO_MANY_FLIGHTS_ERROR] = 1;

    if(checkDateDifference(departureDate, currentDate))
        errors[INVALID_DATE_ERROR] = 1;

    if(((duration[HOUR] == FLIGHT_HOUR_MAX_DURATION) &&
        (duration[MINUTE] > FLIGHT_MINUTE_MAX_DURATION)) ||
       duration[HOUR] > FLIGHT_HOUR_MAX_DURATION)
        errors[INVALID_DURATION_ERROR] = 1;

    if(capacity < FLIGHT_CAPACITY_LOWER_BOUND)
        errors[INVALID_CAPACITY_ERROR] = 1;
}

/* Given two dates, checks if the second date comes before the first one or
 * more than a year into the future. */
int checkDateDifference(int date1[DATE_FIELDS], int date2[DATE_FIELDS]){

    /* First scenario, date1 year comes before date2 or are more than 1
     * year apart. */
    if((date1[YEAR] < date2[YEAR]) || (date1[YEAR] - date2[YEAR] > 1))
        return 1;

    /* Second scenario, date1 year is one unit ahead of date2.  */
    if(date1[YEAR] - date2[YEAR] == 1){
        if(date1[MONTH] > date2[MONTH])
            return 1;
        if((date1[MONTH] == date2[MONTH]) && (date1[DAY] > date2[DAY]))
            return 1;
    }

    /* Third scenario, date1 and date2 occur in the same year. */
    if((date1[YEAR] == date2[YEAR]) && (date1[MONTH] < date2[MONTH]))
        return 1;
    if((date1[YEAR] == date2[YEAR]) && (date1[MONTH] == date2[MONTH])
       && (date1[DAY] < date2[DAY]))
        return 1;

    return 0;
}

/* Prints a flight with its departure date and time data. */
void printDepartureFlight(flight *departureFlight){

    printf("%s %s %02d-%02d-%02d %02d:%02d\n", departureFlight->flightCode,
           departureFlight->arrivalAirport, departureFlight->departureDate[DAY],
           departureFlight->departureDate[MONTH],
           departureFlight->departureDate[YEAR],
           departureFlight->departureTime[HOUR],
           departureFlight->departureTime[MINUTE]);
}

/* Prints a flight with its arrival date and time data. */
void printArrivalFlight(flight *arrivalFlight){

    printf("%s %s %02d-%02d-%02d %02d:%02d\n", arrivalFlight->flightCode,
           arrivalFlight->departureAirport, arrivalFlight->arrivalDate[DAY],
           arrivalFlight->arrivalDate[MONTH], arrivalFlight->arrivalDate[YEAR],
           arrivalFlight->arrivalTime[HOUR], arrivalFlight->arrivalTime[MINUTE]);
}

/* Given two time points, checks which one comes first. Returns 0 if the first
 * time point comes first, 1 otherwise. */
int compareDateAndTime(int date1[DATE_FIELDS], int time1[TIME_FIELDS],
                       int date2[DATE_FIELDS], int time2[TIME_FIELDS]){

    /* Compares the two dates parameters by order of relevance, meaning: year,
     * month, day, hour and minute. */
    if(date1[YEAR] > date2[YEAR])
        return 1;
    if(date1[YEAR] < date2[YEAR])
        return 0;
    if(date1[MONTH] > date2[MONTH])
        return 1;
    if(date1[MONTH] < date2[MONTH])
        return 0;
    if(date1[DAY] > date2[DAY])
        return 1;
    if(date1[DAY] < date2[DAY])
        return 0;

    if(time1[HOUR] > time2[HOUR])
        return 1;
    if(time1[HOUR] < time2[HOUR])
        return 0;
    if(time1[MINUTE] > time2[MINUTE])
        return 1;
    if(time1[MINUTE] < time2[MINUTE])
        return 0;

    return 1;
}

/* Given a flight departure date, time and its duration, computes the
 * corresponding arrival date and time and stores it in the flight data. */
void calculateArrivalDateAndTime(flight *flight){

    int arrivalMinute, arrivalHour, aux, rest = 0;

    /* Initially, adds the minutes of the duration with the departure. */
    aux = flight->duration[MINUTE] +
            flight->departureTime[MINUTE];
    if(aux >= MINUTES_IN_HOUR){
        aux %= MINUTES_IN_HOUR;
        rest = 1;
    }
    arrivalMinute = aux;

    /* Then, the hours. */
    aux = flight->duration[HOUR] +
            flight->departureTime[HOUR] + rest;
    rest = 0;
    if(aux >= HOURS_IN_DAY){
        aux %= HOURS_IN_DAY;
        rest = 1;
    }
    arrivalHour = aux;

    setFlightArrivalTime(flight, arrivalHour, arrivalMinute);

    if(rest == 1){
        /* If the sum of both times goes over 24 hours then increments the
         * arrival date by one. */
        incrementFlightArrivalDate(flight);
    } else {
        setFlightArrivalDate(flight,
                             flight->departureDate[DAY],
                             flight->departureDate[MONTH],
                             flight->departureDate[YEAR]);

    }
}

/* Sets the arrival time of a flight. */
void setFlightArrivalTime(flight *flight, int hour, int minute){

    flight->arrivalTime[HOUR] = hour;
    flight->arrivalTime[MINUTE] = minute;
}

/* Increments a flight arrival date by one */
void incrementFlightArrivalDate(flight *flight){

    /* Depending on the date, the month might change. */
    if(((flight->departureDate[DAY] == 31) &&
        (flight->departureDate[MONTH] == JANUARY ||
                flight->departureDate[MONTH] == MARCH ||
                flight->departureDate[MONTH] == MAY ||
                flight->departureDate[MONTH] == JULY ||
                flight->departureDate[MONTH] == AUGUST ||
                flight->departureDate[MONTH] == OCTOBER)) ||
       ((flight->departureDate[DAY] == 30) &&
        (flight->departureDate[MONTH] == APRIL ||
                flight->departureDate[MONTH] == JUNE ||
                flight->departureDate[MONTH] == SEPTEMBER ||
                flight->departureDate[MONTH] == NOVEMBER))){
        setFlightArrivalDate(flight, 1,
                             flight->departureDate[MONTH] + 1,
                             flight->departureDate[YEAR]);
        return;
    }
    if((flight->departureDate[DAY] == 28) &&
       (flight->departureDate[MONTH] == FEBRUARY)){
        setFlightArrivalDate(flight, 1,
                             flight->departureDate[MONTH] + 1,
                             flight->departureDate[YEAR]);
        return;
    }
    /* New year scenario. */
    if((flight->departureDate[DAY] == 31) &&
       (flight->departureDate[MONTH] == DECEMBER)){
        setFlightArrivalDate(flight, 1, JANUARY,
                             flight->departureDate[YEAR] + 1);
        return;
    }
    /* Usual case. */
    setFlightArrivalDate(flight,
                         flight->departureDate[DAY] + 1,
                         flight->departureDate[MONTH],
                         flight->departureDate[YEAR]);
}

/* Sets the arrival day of a flight. */
void setFlightArrivalDate(flight *flight, int day, int month, int year){

    flight->arrivalDate[DAY] = day;
    flight->arrivalDate[MONTH] = month;
    flight->arrivalDate[YEAR] = year;
}

/* Replaces the system date with a new date. */
void setDate(int oldDate[DATE_FIELDS], int newDate[DATE_FIELDS]){

    oldDate[DAY] = newDate[DAY];
    oldDate[MONTH] = newDate[MONTH];
    oldDate[YEAR] = newDate[YEAR];
}

/* Prints the current system's date. */
void printDate(int date[DATE_FIELDS]){

    printf("%02d-%02d-%02d\n", date[DAY], date[MONTH], date[YEAR]);
}

void printAllFlights(flight *flightList){
    flight *aux;

    for(aux = flightList; aux != NULL; aux = aux->next)
        printFlight(aux);
}

/* Frees all memory reserved to store flight information. */
void freeAllMemory(flight *flightList){
    /* The first iterator checks whether the end of the list has been reached
     * and the second one is used to free the flight's memory. */
    flight *flightIterator1;
    flight *flightIterator2;

    if(flightList == NULL)
        return;

    if(flightList->next == NULL){
        freeAllReservationsMemory(flightList);
        free(flightList);
        return;
    }

    for(flightIterator1 = flightList; flightIterator1 != NULL; ){
        flightIterator2 = flightIterator1;
        flightIterator1 = flightIterator1->next;
        freeAllReservationsMemory(flightIterator2);
        free(flightIterator2);
    }
}

/* Frees all the memory related to a flight's reservations. */
void freeAllReservationsMemory(flight *flight){
    /* The first iterator checks whether the end of the list has been reached
     * and the second one is used to free the flight's memory. */
    reservation *reservationIterator1;
    reservation *reservationIterator2;

    reservationIterator1 = flight->reservations;

    if(reservationIterator1 == NULL)
        return;

    if(reservationIterator1->next == NULL){
        free(reservationIterator1);
        return;
    }

    for( ; reservationIterator1 != NULL; ){
        reservationIterator2 = reservationIterator1;
        reservationIterator1 = reservationIterator1->next;
        free(reservationIterator2);
    }
}

/* Auxiliary functions related to the -r and -e commands. */

/* Gets a flight from the flight list if the flight code and date both match. */
flight *getFlight(flight *flightList, char flightCode[FLIGHT_CODE_MAX_SIZE],
                  int date[DATE_FIELDS]){

    flight *aux;

    for(aux = flightList; aux != NULL; aux = aux->next) {
        if (strcmp(flightCode, aux->flightCode) == 0)
            if(date[DAY] == aux->departureDate[DAY])
                if(date[MONTH] == aux->departureDate[MONTH])
                    if(date[YEAR] == aux->departureDate[YEAR])
                        return aux;
    }

    return NULL;
}

/* Checks whether a reservation code is valid, meaning: is at least 10
 * characters long and contains exclusively upper case letters and numbers.
 * Returns 0 if the code is valid, 1 otherwise. */
int checkReservationCodeValidity(char *reservationCode){

    int i;

    for(i = 0; reservationCode[i] != '\0'; i++){
        if(!((reservationCode[i] >= 'A' &&
              reservationCode[i] <= 'Z') ||
             (reservationCode[i] >= '0' &&
              reservationCode[i] <= '9'))){
            return 1;
        }
    }

    if(i < 10)
        return 1;

    return 0;
}

/* Checks if reservation code was already use.
 * Returns 1 if the code already exists, 0 otherwise. */
int checkReservationCodeExistence(flight *flightList,
                                  char reservationCode
                                  [RESERVATION_CODE_MAX_SIZE]){

    flight *flightIterator;
    reservation *reservationIterator;

    if(flightList == NULL)
        return 0;

    /* Goes through all flights and its lists of reservations and if it finds an
     * equal code stops the search and returns 1. */
    for(flightIterator = flightList; flightIterator != NULL; flightIterator =
        flightIterator->next){
        if(flightIterator->reservations != NULL){
            for(reservationIterator = flightIterator->reservations;
                reservationIterator != NULL; reservationIterator =
                reservationIterator->next){
                    if(strcmp(reservationCode,
                              reservationIterator->reservationCode) == 0){
                        return 1;
                }
            }
        }
    }

    return 0;
}

/* Adds a reservation to a flight. Each flight has a list of reservations which
 * is sorted by the reservation's code lexicographic order. */
void addReservationToFlight(flight *flight,
                            char reservationCode[RESERVATION_CODE_MAX_SIZE],
                            int numberOfPassengers){
    /* Adds the possibility of adding a reservation between two already
     * existing reservations. */
    reservation *beforeReservationIterator;
    reservation *afterReservationIterator;

    reservation *newReservation = (reservation *) calloc(1,
                sizeof(reservation));

    /* Not enough memory. */
    if (newReservation == NULL) {
        printf(NO_MEMORY_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    /* Copies all data onto the reservation to be inserted in the list.*/
    strcpy(newReservation->reservationCode, reservationCode);
    newReservation->numberOfPassengers = numberOfPassengers;
    newReservation->next = NULL;

    /* No reservation booked until now. */
    if(flight->reservations == NULL){
        flight->reservations = newReservation;
        return;
    }

    /* Inserts the reservation at the beginning of the list. */
    if(strcmp(flight->reservations->reservationCode, reservationCode) > 0) {
        newReservation->next = flight->reservations;
        flight->reservations = newReservation;
        return;
    }

    /* Only one reservation present and the new reservation must be added
     * at the end of the list. */
    if(flight->reservations->next == NULL){
        flight->reservations->next = newReservation;
        return;
    }

    /* Inserts the new reservation in the list by alphabetical order. */
    for(beforeReservationIterator = flight->reservations,
        afterReservationIterator = flight->reservations->next;
        beforeReservationIterator != NULL;
        beforeReservationIterator = beforeReservationIterator->next,
        afterReservationIterator = afterReservationIterator->next){

        /* Inserts the reservation at the end of the list*/
        if(afterReservationIterator == NULL){
            beforeReservationIterator->next = newReservation;
            return;
        }

        /* Inserts the reservation between two reservations. */
        if((strcmp(beforeReservationIterator->reservationCode,
                   reservationCode) < 0) &&
           (strcmp(afterReservationIterator->reservationCode,
                   reservationCode) > 0)){
            newReservation->next = afterReservationIterator;
            beforeReservationIterator->next = newReservation;
            return;
        }
    }
}

/* Given a flight, prints all reservations made. */
void printAllFlightReservations(flight *flight){
    reservation *reservationIterator;

    if(flight->reservations == NULL)
        return;

    for(reservationIterator = flight->reservations;
        reservationIterator != NULL; reservationIterator =
        reservationIterator->next){
        printf("%s %d\n", reservationIterator->reservationCode,
               reservationIterator->numberOfPassengers);
    }
}

/* Checks whether a flight with a given flight code is stored. Returns 0 if not,
 * 1 otherwise. */
int checkFlightExistence(flight *flightList,
                         char flightCode[FLIGHT_CODE_MAX_SIZE]){
    flight *flightIterator;

    if(flightList == NULL)
        return 0;

    for(flightIterator = flightList; flightIterator != NULL;
        flightIterator = flightIterator->next) {
            if (strcmp(flightIterator->flightCode, flightCode) == 0)
                return 1;
    }

    return 0;
}

/* Deletes a reservation code by finding its belonging list among all flights
 * and decreases the flight's number of passengers. */
void eliminateReservation(flight *flightList,
                          char reservationCode[RESERVATION_CODE_MAX_SIZE]){

    flight *flightIterator;
    reservation *beforeReservationIterator;
    reservation *afterReservationIterator;

    if(flightList == NULL)
        return;

    /* Goes through each reservation list of all flights. */
    for(flightIterator = flightList; flightIterator != NULL; flightIterator =
        flightIterator->next){
        if(flightIterator->reservations != NULL){

            /* Removes, if matched, the first reservation on the flight list. */
            if(strcmp(flightIterator->reservations->reservationCode,
                      reservationCode) == 0){

                beforeReservationIterator = flightIterator->reservations;
                flightIterator->reservations =
                flightIterator->reservations->next;

                /* Decreases the flight's number of reservations. */
                flightIterator->numberOfPassengers -=
                beforeReservationIterator->numberOfPassengers;
                free(beforeReservationIterator);
                return;
            }

            /* More than one reservation. Removes it if it finds it. */
            for(beforeReservationIterator = flightIterator->reservations,
                afterReservationIterator = flightIterator->reservations->next;
                afterReservationIterator != NULL;
                beforeReservationIterator = beforeReservationIterator->next,
                afterReservationIterator = afterReservationIterator->next){

                if(strcmp(afterReservationIterator->reservationCode,
                          reservationCode) == 0){
                    beforeReservationIterator->next =
                    afterReservationIterator->next;

                    /* Decreases the flight's number of reservations. */
                    flightIterator->numberOfPassengers -=
                    afterReservationIterator->numberOfPassengers;
                    free(afterReservationIterator);
                    return;
                }
            }
        }
    }
}

/* Eliminates one or more flights out of the flight list. Returns a pointer,
 * which may be new, to the beginning of the list. */
flight *eliminateFlight(flight *flightList, int *numberOfFlights,
                        char flightCode[FLIGHT_CODE_MAX_SIZE]){

    flight *beforeFlightIterator;
    flight *destroyFlight;
    flight *afterFlightIterator;
    int firstElementDeleted;
    firstElementDeleted = 1;

    if(flightList == NULL)
        return flightList;

    /* Repeatedly removes the first element of the list if multiple flights
     * in the beginning are to be deleted. */
    while(firstElementDeleted) {
        if(flightList == NULL)
            return flightList;
        if (strcmp(flightList->flightCode, flightCode) == 0) {
            afterFlightIterator = flightList->next;
            freeAllReservationsMemory(flightList);
            free(flightList);
            (*numberOfFlights)--;
            flightList = afterFlightIterator;
            firstElementDeleted = 1;
        } else
            firstElementDeleted = 0;
    }

    /* Remove flights on the list. */
    for(beforeFlightIterator = flightList,
        afterFlightIterator = flightList->next;
        afterFlightIterator != NULL;
        ){

        if(strcmp(afterFlightIterator->flightCode,
                  flightCode) == 0){
            beforeFlightIterator->next = afterFlightIterator->next;

            destroyFlight = afterFlightIterator;

            afterFlightIterator = beforeFlightIterator->next;

            freeAllReservationsMemory(destroyFlight);
            free(destroyFlight);
            (*numberOfFlights)--;
        } else{
            beforeFlightIterator = beforeFlightIterator->next;
            afterFlightIterator = afterFlightIterator->next;
        }
    }

    return flightList;
}
