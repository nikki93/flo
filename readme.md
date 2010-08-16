# flo

Organize events, to-dos, and deadlines.

## Installation

    make
    cp flo /usr/local/bin/

## Example

    $ flo watch movie,t020
       0     today 20:00  watch movie
    $ flo eat lunch,t11100-t11130
       0     today 20:00  watch movie
       1  tomorrow 11:00  eat lunch
                   11:30

## Usage

### Add item

    flo [.tag] what[,from][-to]

    flo -w what [-f from | -t to] [-T tag]

### List items

    flo

### List items of a tag

    flo .tag

### Remove item

    flo -r id

### Change item

For fields other than `-w what`, `r` as value removes the field.

    flo -c id -w what | -f from | -t to | -T tag

### Date formats

    YYYYMMDDhhmm
        MMDDhhmm
          DDhhmm
          DDhh
          DD

If the year or the month isn’t specified, the current year and month is used.
For formats without a month, if the date specified is before today’s date, the
month is set to the next month.

Replace `DD` with `tn` to set the date `n` days from today.

The value for hours and minutes is set to `00` if no other value is specified.

## Items file

Items are written to `$HOME/.flo`. Each item is stored on a separate line, and
each field is separated by a tab.

    what\tfrom\tto\ttag

## License

Copyright © 2010, Alexander Teinum <ateinum@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright notice
and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
