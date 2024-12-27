FROM ubuntu:24.04



RUN apt-get update && apt-get install -y git build-essential cmake gcc

WORKDIR /app



RUN git clone https://github.com/open-source-parsers/jsoncpp && \
    cd jsoncpp && mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=ON -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" .. && \
    make && make install 

ENV LD_LIBRARY_PATH="/usr/local/lib:${LD_LIBRARY_PATH}"

COPY . /app

RUN mkdir build && cd build && cmake .. && make

EXPOSE 8080

CMD [ "/app/build/proj" ]