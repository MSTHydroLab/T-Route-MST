# Using Modified T-Route with NextGen in a Box - Setup and Calibration Guide

## Overview

This guide provides step-by-step instructions for setting up and using the modified T-Route with NextGen in a Box (NGIAB) locally on your computer. The modified version enables flowpath-level routing and enhanced data assimilation capabilities for improved hydrological modeling and calibration.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Part 1: Local Setup with Modified T-Route](#part-1-local-setup-with-modified-troute)
- [Part 2: Running NextGen in a Box](#part-2-running-nextgen-in-a-box)
- [Part 3: Calibration with ngen-cal](#part-3-calibration-with-ngen-cal)

---

## Prerequisites

Ensure you have the following installed on your system:

- **Docker** (version 20.10 or higher)
- **Docker Buildx** (for multi-platform builds)
- **Git**
- **Python 3.10+** (for data extraction tools)
- **uvx** (Python package runner)

### Install uvx (if not already installed)

```bash
pip install uv
```

---

## Part 1: Local Setup with Modified T-Route

### Step 1: Clone NGIAB Cloud Infrastructure Repository

```bash
git clone https://github.com/CIROH-UA/NGIAB-CloudInfra.git
cd NGIAB-CloudInfra
```

### Step 2: Modify Dockerfile with MSTHydroLab Repositories

Navigate to the Docker directory and open the Dockerfile:

```bash
cd docker
```

Locate the following environment variables in the `Dockerfile`:

```dockerfile
ENV TROUTE_REPO=CIROH-UA/t-route
ENV TROUTE_BRANCH=ngiab
ENV NGEN_REPO=CIROH-UA/ngen
ENV NGEN_BRANCH=ngiab
```

**Replace them with:**

```dockerfile
ENV TROUTE_REPO=said-uzzaman/t-route-MST
ENV TROUTE_BRANCH=main
ENV NGEN_REPO=said-uzzaman/ngen
ENV NGEN_BRANCH=main
```

> **Note:** Adjust the branch names (`main`) if your modified repositories use different branch names (e.g., `ngiab`, `master`, or `develop`).

### Step 3: Build the Custom Docker Image

Build the Docker image with the modified T-Route and ngen:

```bash
docker buildx build -f Dockerfile -t awiciroh/ciroh-ngen-image .
```

**Verify the build:**

```bash
docker images | grep ciroh-ngen-image
```

You should see an image tagged as `awiciroh/ciroh-ngen-image`.

---

## Part 2: Running NextGen in a Box

### Step 1: Extract Hydrofabric and Forcing Data

Use `ngiab-prep` to extract data for your area of interest. In this example, we'll use USGS gage `05454300` (Clear Creek watershed).

```bash
uvx -p 3.10 ngiab-prep \
  -i gage-05454300 \
  -sfr \
  --start 2017-01-01 \
  --end 2020-12-31 \
  -o ck_2018_2020 \
  --source aorc
```

**Parameters:**
- `-i gage-05454300`: Input gage ID
- `-sfr`: Include subset flowpaths and reaches
- `--start` / `--end`: Simulation time period
- `-o ck_2018_2020`: Output directory name
- `--source aorc`: Forcing data source (AORC - Analysis of Record for Calibration)


### Step 2: Configure T-Route for Catchment-Level Routing

Edit the T-Route configuration file to enable catchment file processing:

```bash
cd ck_2018_2020/config
nano troute.yaml
```

**Update the forcing parameters:**

```yaml
forcing_parameters:
    qts_subdivisions: 12
    dt: 300  # Time step in seconds (5 minutes)
    qlat_input_folder: ./outputs/ngen/
    qlat_file_pattern_filter: "cat-*"  # Enable catchment-level routing
    nts: 315072  # Total number of time steps
    max_loop_size: 315072
```

### Step 3: Run NextGen in a Box

Navigate back to the NGIAB directory and run the simulation:


```bash
# Run ngen
./guide.sh
```


## Part 3: Calibration with ngen-cal

### Step 1: Clone ngen-cal Repository

```bash
git clone -b ngiab_cal https://github.com/CIROH-UA/ngen-cal.git
cd ngen-cal
```

### Step 2: Build Custom Calibration Docker Image

```bash
docker build -t ngiab-cal:custom .
```

### Step 3: Prepare Calibration Configuration

Extract calibration-specific configuration from your data directory:

```bash
uvx ngiab-cal /home/msmg8/NextGen/clear_creek_data/ck_2018_2020/ \
  -g 05454300 \
  -i 200 \
  -w 365 \
  --cr 1
```

**Parameters:**
- `/home/msmg8/NextGen/clear_creek_data/ck_2018_2020/`: Path to your NGIAB data directory
- `-g 05454300`: Gage ID for calibration target
- `-i 200`: Number of calibration iterations
- `-w 365`: Warm-up period in days
- `--cr 1`: Calibration ratio


### Step 4: Run Calibration

Launch the calibration container:

```bash
docker run -it \
  -v "/home/msmg8/NextGen/clear_creek_data/ck_2018_2020/:/ngen/ngen/data" \
  --user $(id -u):$(id -g) \
  ngiab-cal:custom
```

**Inside the container, start calibration:**

```bash
./run.sh
```

## Example: Complete Workflow

Here's a complete example for the Clear Creek watershed:

```bash
# 1. Setup
git clone https://github.com/CIROH-UA/NGIAB-CloudInfra.git
cd NGIAB-CloudInfra/docker

# 2. Modify Dockerfile (update ENV variables as shown above)
nano Dockerfile

# 3. Build custom image
docker buildx build -f Dockerfile -t awiciroh/ciroh-ngen-image .

# 4. Extract data
cd ../..
uvx -p 3.10 ngiab-prep -i gage-05454300 -sfr \
  --start 2017-01-01 --end 2020-12-31 \
  -o ck_2018_2020 --source aorc

# 5. Update T-Route config
nano ck_2018_2020/config/troute.yaml
# Set: qlat_file_pattern_filter: "cat-*"


# 7. Clone and build ngen-cal
git clone -b ngiab_cal https://github.com/CIROH-UA/ngen-cal.git
cd ngen-cal
docker build -t ngiab-cal:custom .

# 8. Prepare calibration
cd ..
uvx ngiab-cal ck_2018_2020/ -g 05454300 -i 200 -w 365 --cr 1

# 9. Run calibration
docker run -it -v "$(pwd)/ck_2018_2020:/ngen/ngen/data" \
  --user $(id -u):$(id -g) ngiab-cal:custom

# Inside container:
./run.sh

---
## Authors

**Md Saiduzzaman**  
Graduate Research Assistant  
Department of Civil, Architectural and Environmental Engineering  
Missouri University of Science & Technology  

**Dr. Bong-Chul Seo**  
Assistant Professor  
Department of Civil, Architectural and Environmental Engineering  
Missouri University of Science & Technology  

---
