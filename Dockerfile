# =========================
# Build stage
# - This stage contains all build tools (compiler, CMake, headers).
# - We build the Runner binary here, then copy only the binary to the runtime stage.
# =========================
FROM ubuntu:24.04 AS build

# Install build tools and build-time dependencies.
# --no-install-recommends keeps the image smaller by skipping "recommended" packages.
# We also remove apt cache to reduce the final layer size.
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake \
    # TODO(1): 追加で必要なら ninja-build とか依存 \
    nlohmann-json3-dev \
 && rm -rf /var/lib/apt/lists/*

# Set working directory inside the container for building sources.
WORKDIR /src

# Copy the entire repository into the container.
# Note: use .dockerignore to exclude build/ and other artifacts to avoid stale CMake caches.
COPY . .

# Build the Runner inside the container.
# We remove any existing build directory to avoid CMake cache path mismatches.
# -S .  : source directory
# -B build : build output directory
# Release  : optimized build (smaller/faster than Debug)
RUN rm -rf build \
 && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j

# =========================
# Runtime stage
# - This stage is kept minimal: only what is needed to run the binary.
# - No compiler, no CMake, no source code.
# =========================
FROM ubuntu:24.04

# Install runtime basics.
# ca-certificates is useful for HTTPS access (e.g., talking to Azure services).
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*


# Set the default working directory at runtime.
# We mount a host folder to /work and read/write input/output files there.
WORKDIR /work

# Copy only the built Runner binary from the build stage.
# This keeps the runtime image smaller and cleaner.
COPY --from=build /src/build/DiscreteTimeSystemRunner /app/DiscreteTimeSystemRunner


# Make the Runner the default command when the container starts.
# Any extra arguments passed to `docker run` will be forwarded to the Runner.
ENTRYPOINT ["/app/DiscreteTimeSystemRunner"]

