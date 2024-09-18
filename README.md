# fdapdepy: A Python wrapper for the C++ library fdaPDE

The package was developed using [pybind11](https://github.com/pybind/pybind11) and follows the [Scientific Python Library Development Guide](https://learn.scientific-python.org/development/).
It is part of the PhD project for the course [Python Driving Licence](https://www11.ceda.polimi.it/schedaincarico/schedaincarico/controller/scheda_pubblica/SchedaPublic.do?&evn_default=evento&c_classe=802611&lang=IT&__pj0=0&__pj1=ba8223ddc8f86e609d586a38f93a860c) at Politecnico di Milano 

## Theoretical framework
    ...
    
## Build Requirements 
To build the package you will need:
* A C++20 compliant compiler. Supported versions are:
     * Linux: `gcc` 11 (or higher), `clang` 15 (or higher)
	 * macOS: `apple-clang` (the XCode version of `clang`, AppleClang 15 or higher).
* The [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) linear algebra library, version 3.4.0.
* python 3.7 or higher

## Installation in a Virtual Python Environment
To install the package and run the package in a virtual python environment:
1. Clone the repo:
    ```
    git clone --recurse-submodules https://github.com/aldoclemente/fdapde-py.git
    cd fdapde-py/
    ```
2. Create a virtual python environment and install the package:
    ```
    python3 -m venv .venv
    . .venv/bin/activate
    ./install.sh 
    ```
3. Run the available jupyter notebooks within the virtual environment. For instance, run from the terminal:
    ```
    jupyter notebook tests/sr_pde.ipynb
    ```

