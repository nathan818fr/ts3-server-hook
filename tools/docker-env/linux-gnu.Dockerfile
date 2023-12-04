FROM debian:buster

# Create Debian Buster (glibc 2.28) image for building linux GNU binaries

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -yqq --no-install-recommends \
        build-essential \
        ca-certificates \
        wget \
        git \
    && rm -rf /var/lib/apt/lists/*

RUN wget -O- "https://github.com/Kitware/CMake/releases/download/v3.27.8/cmake-3.27.8-linux-$(uname -m).tar.gz" \
      | tar -xz --strip-components=1 -C /

RUN groupadd -g 911 build \
    && useradd -mN -u 911 -g 911 build

COPY --chmod=700 entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
