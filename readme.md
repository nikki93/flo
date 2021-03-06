# flo

Organize events, to-dos, and deadlines.

## Installation

    make
    make install

## Example

    $ flo watch movie,d020
    Ids are updated.
    $ flo eat lunch,d11100-d11130
    Ids are updated.
    $ flo
       0  08-16 20:00  d0  watch movie
       1  08-17 11:00  d1  eat lunch
                11:30  d1

## Usage

    flo [-a] [-c id] [-f from] [-r id] [-t to] [-w what] [what[,from][-to]]

## Options

    -a         all items
    -c id      change item
    -f from    from date
    -r id      remove item
    -t to      to date
    -w what

When changing an item, setting `-f` or `-t` to `r` removes the field.

### Date formats

`YYYYMMDDhhmm`, `MMDDhhmm`, `DDhhmm`, `DDhh`, and `DD` are the valid date
formats. Replace `DD` with `dn` to set the date `n` days from today’s date.

If the year or the month isn’t specified, the current year and month is used.
For formats without a month, if the date specified is before today’s date, the
month is set to the next month.

The value for hours and minutes is set to `00` if no other value is specified.

## Items file

Items are written to `$HOME/.flo`. Each item is stored on a separate line, and
each field is separated by a tab.

    what\tfrom\tto\n

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
