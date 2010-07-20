# flo

flo is a command line application that keeps you in check. It’s small and fast.

## installation

    make
    cp flo /usr/local/bin/

## examples

    $ flo -w "clean apartment"
    $ flo -w "eat lunch" -a "some cafe" -f 181430 -t 181530
    $ flo -w "a deadline" -a "uni" -t 10011200

    $ flo
       0  2010-07-18 14:30  eat lunch@some cafe
          2010-07-18 15:30
    d  1  2010-10-01 12:00  a deadline
       2  clean apartment

## usage

    flo [-w what [-a at | -f from | -t to]]

### date format

These are the valid date formats that can be passed to `-f` and `-t`:

    YYMMDDhhmm
      MMDDhhmm
        DDhhmm
        DDhh
        DD

For the shorter formats, if the year or the month isn’t specified, the current
year and month is used. For formats without a month, if the date specified is
before the current date, the month is set to the next month.

The value for hours and minutes is set to `00` if no other value is specified.

## license

ISC.
