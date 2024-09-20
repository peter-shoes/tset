<img src="https://github.com/user-attachments/assets/da07b87b-3ca1-4870-a062-73068ba81364" alt="LOGO" width="100"/>

# TSET LaTeX preprocessor
TSET is a preprocessor for latex based on the syntax of the T language. The T language includes a number of features including:
 - Natural macros with no escapes
 - Simple table importing from CSV
 - Automatic line breaks in math mode

TSET is licensed under the [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.en.html), so no funny business.

## Build using GNUmake
Just run `make`

## Install
Install by running `sudo make install` or by copying the built binary to your /bin or /usr/bin directory.

## Testing
Testing is handled by [DejaGNU](https://www.gnu.org/software/dejagnu/), and tests can be found in the `tests` directory.

## Running TSET
Once you have built (and optionally installed) TSET, run by using the following command:

    $ tset <path-to-T-file>.t <path-to-output-file>.tex
If you do not specify an output file path, the processed T file will be written to stdout.

# Language Specification
The full language specification document can be found in the included manpage file, but a basic overview is included here
## Macros
In order to set up a macro, use the following format for a plaintext macro:

    %def <macro> <macrobody>
If you would like to do the same for math mode, use the following format:

    %mathdef <macro> <macrobody>
It should be noted that you can also use delimiters if you wish, like in the following examples:

    %def this that
    %def "Another Thing" {A different thing}
Finally, if you wish to match any word or set of words (a string), you can specify that with the `#w` and `#s` tokens respectively. For example, the def:

    %def #w/#w $\frac{#1}{#2}$
would match

    This is a fraction: 3/4
and would become:

    This is a fraction: $\frac{3}{4}$

## Tables
Tables are very straightforward, simply use the format:

    %table <path-to-table>.csv

# Maintenance and Contribution
This project is currently being built to a version 1.0, and as such contributions are not accepted at this time. This work is anticipated to be completed by 12/24, at which time we will open the issues page for contributions and bug reports.
This project will be maintained after 12/24 until I get bored or fed up. It should not need much maintenance, as it does not depend on any external libraries. In the case that LaTeX itself undergoes some sort of format change that effects TSET, I will do my best to update the work.
If anyone is interested in maintaining a package for their favorite distribution, please get in contact with me here. I will likely be maintaining at least an RPM for this work.
## Support for Alternative Operating Systems
I personally do not believe in Windows as a serious platform for use by anyone, but the world does not revolve around me. For that reason, I intend to release precompiled binaries for Windows systems, with a rudimentary GUI, as PowerShell and CygWin are not very good. 
In regards to OSX, I feel that the terminal is sufficient, and users should easily be able to install gcc and DejaGNU if so desired. 
