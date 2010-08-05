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

    $ f deliver assignment-10011200
    d  0  2010-10-01 12:00  deliver assignment
    t  1  clean apartment

### add tagged event

    $ f @work eat lunch,061100-061130
       0       today 11:00  @work eat lunch
                     11:30
    d  1  2010-10-01 12:00  deliver assignment
    t  2  clean apartment

### add event

    $ f watch movie,061900
       0       today 11:00  @work eat lunch
                     11:30
       1       today 19:00  watch movie
    d  2  2010-10-01 12:00  deliver assignment
    t  3  clean apartment

### change the time of the event

    $ f -c 1 -f 062000
       0       today 11:00  @work eat lunch
                     11:30
       1       today 20:00  watch movie
    d  2  2010-10-01 12:00  deliver assignment
    t  3  clean apartment

### remove todo

    $ f -r 3
       0       today 11:00  @work eat lunch
                     11:30
       1       today 20:00  watch movie
    d  2  2010-10-01 12:00  deliver assignment

### change deadline into todo

    $ f -c 2 -t r
       0       today 11:00  @work eat lunch
                     11:30
       1       today 20:00  watch movie
    t  2  deliver assignment

### show items tagged with “work”

    $ f @work
       0       today 11:00  eat lunch
                     11:30

## usage

    flo [@tag |what[,from][-to] || [-c id] [-T tag] -w what [-f from | -t to] || -r id]

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
