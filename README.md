<img src="https://github.com/user-attachments/assets/da07b87b-3ca1-4870-a062-73068ba81364" alt="LOGO" width="100"/>

# TSET LaTeX preprocessor
TSET is a preprocessor for latex based on the syntax of the T language. The T language includes a number of features including:
 - Natural macros with no escapes
 - Simple table importing from CSV
 - Automatic line breaks in math mode
 - Automatic replacements for common sense TeX features

TSET is licensed under the [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.en.html), so no funny business.

# Building TSET on your system
The authors of TSET do not provide compiled binaries for TSET, as building it is very simple on any machine.
Below are some instructions on how to build for common operating systems, as well as some required packages.

## Linux
TSET is designed to run on Linux first as a bash utility. If you are using linux, you should know how to build the binary, but here are some simple instructions anyway:

Start by cloning the repo:
```
git clone https://github.com/peter-shoes/tset.git
```
next, cd into the new dir and run make
```
cd tset && make
```
Finally, if you's like to install TSET to your system, run the install recipe
```
sudo make install
```
Great! now you have TSET installed in your system.

## MacOS
MacOS is very similar to Linux, so the above steps for linux should work just fine for your MacOS system. Just open your terminal and be sure that you have both `git` and `make` installed through homebrew, which is the default MacOS packaging system.

## Windows
Getting TSET running natively on a Windows system is possible, but I personally do not know anything about the Windows compiler toolchain provided by Microsoft. You can probably use LLVM if you like, but the included makefile only contains gcc recipes.
As an alternative approach, I recomend that you install either pure [MinGW](https://www.mingw-w64.org/) or the derivative project [Cygwin](https://cygwin.com/). Once you have one of these installed, you'll then need to run the provided terminal and be sure that you have the appropriate toolchains present. You have the option of installing a package manager like [apt-cyg](https://github.com/transcode-open/apt-cyg) for Cygwin, and then running
```
sudo apt-cyg install git
```
At this point, you can simply follow the above linux instructions.

### Installing on Windows the low-tech way

However, if this is above your experience level, I would reccomend simply pulling the current version of this repository using wget, unzipping it, and then building using the above linux instructions.
To do this, simply run the following:
```
wget https://github.com/peter-shoes/tset/archive/refs/heads/main.zip && unzip main.zip && mv tset-main tset
```
This will create a folder based off of the main branch of TSET.

# Testing
Testing is handled by a small bash script that diffs output, and tests can be found in the `testsuite` directory, separated by feature.
If you want to add a new test, simply create a T file and the expected TeX file to be output, give them the same name, and place them in the appropriate folder.

# Running TSET
Once you have built (and optionally installed) TSET, run by using the following command:

    $ tset <path-to-T-file>.t <path-to-output-file>.tex
If you do not specify an output file path, the processed T file will be written to stdout.

# Language Specification
The full language specification document can be found in the included Tex file, but a basic overview is included here
## Macros
In order to set up a macro, use the following format for a plaintext macro:

    %def <macrohead> <macrobody>
If you would like to do the same for math mode, use the following format:

    %mathdef <macrohead> <macrobody>
It should be noted that you can also use delimiters if you wish, like in the following examples:

    %def this that
    %def "Another Thing" {A different thing}
Finally, if you wish to match any word or set of words (a string), you can specify that with the `#w`, `#n`, and `#s` tokens respectively. For example, the def:
```
%def #n/#n $\frac{#1}{#2}$
```
would match
```
This is a fraction: 3/4
```
and would become:
```
This is a fraction: $\frac{3}{4}$
```
Note that becase we are using the `#n` wildcard rather than the `#w` wildcard, only numbers would be recognized. Therefore a string like:
```
you can have chicken/fish.
```
would not cintain a match
## Tables
Tables are very straightforward, simply use the format:
```
%table <path-to-table>.csv
```
## Passive Additions
TSET contains a number of what could be considered "passive additions" to the TeX language. These are things that the user does not need to ask for and are implemented automatically. For example, all lines in math mode that are grouped using brackets are given hinting wrap commands that will break the line in an appropriate place if the TeX compiler decided a linebreak is needed. Additionally, sub and superscripts are automatically bundled, so a typed `2^100` becomes `2^{100}`.
# Maintenance and Contribution
This project is currently being built to a version 1.0, and as such contributions are not accepted at this time. This work is anticipated to be completed by 12/24, at which time we will open the issues page for contributions and bug reports.
This project will be maintained after 12/24. It should not need much maintenance, as it does not depend on any external libraries. In the case that LaTeX itself undergoes some sort of format change that effects TSET, I will do my best to update the work.
If anyone is interested in maintaining a package for their favorite distribution, please get in contact with me here. I will likely be maintaining at least an RPM for this work.
