rfc 7230

3.  Message Format
...
A sender MUST NOT send whitespace between the start-line and the
   first header field.  A recipient that receives whitespace between the
   start-line and the first header field MUST either reject the message
   as invalid or consume each whitespace-preceded line without further
   processing of it (i.e., ignore the entire line, along with any
   subsequent lines preceded by whitespace, until a properly formed
   header field is received or the header section is terminated).
...

start-line     = request-line (for requests) / status-line (for response)


HTTP does not place a predefined limit on the length of a
   request-line, as described in Section 2.5.  A server that receives a
   method longer than any that it implements SHOULD respond with a 501
   (Not Implemented) status code.  A server that receives a



3.1.1.  Request Line

...

HTTP does not place a predefined limit on the length of a
request-line, as described in Section 2.5.  A server that receives a
method longer than any that it implements SHOULD respond with a 501
(Not Implemented) status code.  A server that receives a request-target longer than any URI it wishes to parse MUST respond
   with a 414 (URI Too Long) status code (see Section 6.5.12 of
   [RFC7231]).

... It is RECOMMENDED that all HTTP senders and recipients
   support, at a minimum, request-line lengths of 8000 octets.

3.2.  Header Fields

Each header field consists of a case-insensitive field name followed
by a colon (":"), optional leading whitespace, the field value, and
optional trailing whitespace.

header-field   = field-name ":" OWS field-value OWS

3.2.2.  Field Order

...
A sender MUST NOT generate multiple header fields with the same field
name in a message unless either the entire field value for that
header field is defined as a comma-separated list [i.e., #(values)]
or the header field is a well-known exception (as noted below).

...
If a message is received that has multiple Content-Length header
fields with field-values consisting of the same decimal value, or a
single Content-Length header field with a field value containing a
list of identical decimal values (e.g., `Content-Length: 42, 42`),
indicating that duplicate Content-Length header fields have been
generated or combined by an upstream message processor, then the
recipient MUST either reject the message as invalid or replace the
duplicated field-values with a single valid Content-Length field
containing that decimal value prior to determining the message body
length or forwarding the message.

5.4 Host

A server MUST respond with a 400 (Bad Request) status code to any
HTTP/1.1 request message that lacks a Host header field and to any
request message that contains more than one Host header field or a
Host header field with an invalid field-value.


RFC 7231


3.1.1.  Processing Representation Data

3.1.1.1.  Media Type
HTTP uses Internet media types [RFC2046] in the Content-Type
(Section 3.1.1.5) and Accept (Section 5.3.2) header fields in order
to provide open and extensible data typing and type negotiation.
Media types define both a data format and various processing models:
how to process that data in accordance with each context in which it
is received.

    media-type = type "/" subtype *( OWS ";" OWS parameter )
    type       = token
    subtype    = token
...

3.1.1.5.  Content-Type
...
Content-Type = media-type. ( it is not show as a list)

👉 Headers such as Content-Type are not explicitly labeled “singleton” in RFC 7230.
Instead:
Their grammar (defined in RFC 7231) defines them as single-value fields
Since they are not comma-list headers, Section 3.2.2 implies:
    → they must not be duplicated

4.  Request Methods

4.1.  Overview

All general-purpose servers MUST support the methods GET and HEAD.
   All other methods are OPTIONAL.
