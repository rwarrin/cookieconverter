# Covert Cookies

Covert cookies to netscape format.

## Usage

`convertcookies.exe [filename] [cookie count (opt)]`

Filename is a file containing cookies copied out of the cookies view in chrome
developer tools. Cookie count is an optional value, if not provided 512 cookies
are assumed by default, this is only used for allocating memory and the value
only needs to be greater than or equal to the number of cookies to be converted.

## build

# Make sure you have msvc build tools available on the command line.
# From the root directory run `code\build.bat`.
