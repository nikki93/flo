# flo

flo is a command line application that keeps you in check. It’s small and fast.

## installation

    make
    cp flo /usr/local/bin/

## examples

### add todo

 $ flo -w "clean apartment"
   0  clean apartment

### add event

 $ flo -w "eat lunch" -a "some cafe" -f 221200 -t 221245
   0  2010-07-22 12:00  eat lunch
      2010-07-22 12:45
   1  clean apartment

### add deadline

 $ flo -w "a deadline" -a "uni" -t 10011200
   0  2010-07-22 12:00  eat lunch
      2010-07-22 12:45
d  1  2010-10-01 12:00  a deadline
   2  clean apartment

### change item

 $ flo -c 0 -f 221300 -t 221345
   0  2010-07-22 13:00  eat lunch
      2010-07-22 13:45
d  1  2010-10-01 12:00  a deadline
   2  clean apartment

### remove item

 $ flo -r 2
   0  2010-07-22 13:00  eat lunch
      2010-07-22 13:45
d  1  2010-10-01 12:00  a deadline

## usage

    flo [-r id || [-c id] -w what | -a at | -f from | -t to]

### date format

These are the date formats that can be passed to `-f` and `-t`:

    YYMMDDhhmm
      MMDDhhmm
        DDhhmm
        DDhh
        DD

If the year or the month isn’t specified, the current year and month is used.
For formats without a month, if the date specified is before today’s date, the
month is set to the next month.

The value for hours and minutes is set to `00` if no other value is specified.

## license

ISC.
