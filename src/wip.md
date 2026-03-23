while (bytes_received) {
    appendData(buffer, bytes_received);

    while (true) {
        ResultType result = parseNext();

        if (result == ERROR) {
            // syntactic error → 400
            HttpResponse response = ResponseBuilder::buildBadRequest(request_parser_.getRequest());
            serialized_response_ = response.serialize();
            setWantWrite(true);
            reactor_.updateHandler(this);
            return;
        }
        else if (result == AGAIN) break;

        else if (result == OK && request_parser_.isComplete()) {
            // request is syntactically OK
            ServerConfig server = ServerMatcher.matchServer(...);
            LocationConfig location = LocationMatcher.matchLocation(...);

            if (!location.isMethodAllowed(request_parser_.getMethod())) {
                // semantic error → 405
                HttpResponse response = ResponseBuilder::buildMethodNotAllowed(request_parser_.getRequest());
                serialized_response_ = response.serialize();
                setWantWrite(true);
                reactor_.updateHandler(this);
                return;
            }

            // normal request processing
            HttpResponse response = ResponseBuilder::build(request_parser_.getRequest(), server, location);
            serialized_response_ = response.serialize();
            setWantWrite(true);
            reactor_.updateHandler(this);
            return;
        }
    }
}
