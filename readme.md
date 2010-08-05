# flo

flo is a command line application that keeps you in check. It’s small and fast.

## installation

    make
    cp flo /usr/local/bin/

## getting started

### add todo

     $ flo clean apartment
     t  0  clean apartment

### add event

     $ flo eat lunch@some cafe,221200-221245
        0       today 12:00  eat lunch@some cafe
                      12:45
     t  1  clean apartment

### add deadline

     $ flo a deadline@uni-10011200
        0       today 12:00  eat lunch@some cafe
                      12:45
     d  1  2010-10-01 12:00  deliver assignment
     t  2  clean apartment

### change the time of the lunch

     $ flo -c 0 -f 221300 -t 221345
        0       today 13:00  eat lunch@some cafe
                      13:45
     d  1  2010-10-01 12:00  deliver assignment
     t  2  clean apartment

### remove todo

     $ flo -r 2
        0       today 13:00  eat lunch@some cafe
                      13:45
     d  1  2010-10-01 12:00  deliver assignment

### change the deadline into a todo

    $ flo -c 1 -t r
        0       today 13:00  eat lunch@some cafe
                      13:45
        1  deliver assignment

## usage

    flo [what[@at][,from][-to] || [-c id] -w what | -a at | -f from | -t to || -r id]

### date formats

    YYMMDDhhmm
      MMDDhhmm
        DDhhmm
        DDhh
        DD

If the year or the month isn’t specified, the current year and month is used.
For formats without a month, if the date specified is before today’s date, the
month is set to the next month.

The value for hours and minutes is set to `00` if no other value is specified.

When changing an existing item, setting a date to `r` removes the date.

## license

ISC.
