# flo

flo is a command line application that keeps you in check. It’s small and fast.

## installation

    make
    cp flo /usr/local/bin/

Optionally set up `f` as an alias to `flo`.

    echo "alias f='flo'" >> ~/.bashrc

## getting started

### add todo

    $ f clean apartment
    t  0  clean apartment

### add deadline

    $ f deliver assignment@uni-10011200
    d  0  2010-10-01 12:00  deliver assignment@uni
    t  1  clean apartment

### add event

    $ f eat lunch@some cafe,051200-051245
       0       today 12:00  eat lunch@some cafe
                     12:45
    d  1  2010-10-01 12:00  deliver assignment@uni
    t  2  clean apartment

### add another event

    $ f watch movie,051900
       0       today 12:00  eat lunch@some cafe
                     12:45
       1       today 19:00  watch movie
    d  2  2010-10-01 12:00  deliver assignment@uni
    t  3  clean apartment

### change the time of the event

    $ f -c 1 -f 052000
       0       today 12:00  eat lunch@some cafe
                     12:45
       1       today 20:00  watch movie
    d  2  2010-10-01 12:00  deliver assignment@uni
    t  3  clean apartment

### remove todo

    $ f -r 3
       0       today 12:00  eat lunch@some cafe
                     12:45
       1       today 20:00  watch movie
    d  2  2010-10-01 12:00  deliver assignment@uni

### change deadline into todo

    $ f -c 2 -t r
       0       today 12:00  eat lunch@some cafe
                     12:45
       1       today 20:00  watch movie
    t  2  deliver assignment@uni

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

## aliases

    # all todos
    alias ft='flo | grep ^t' 

    # all deadlines
    alias fd='flo | grep ^d' 

    # all events
    alias fe='flo | grep ^[^td]' 

    # today’s events and deadlines
    alias f0='flo | grep today' 

    # today’s events and deadlines, all todos
    alias f0t='flo | grep -E "today|^t"' 

## license

ISC.
