# Generating route distributions
Using conda package manager, run `conda create -f environment.yml` to install dependencies. Activate the environment with `conda activate sumolib`.

Run `$SUMO_HOME/tools/findAllRoutes.py -c source_routes.cfg` to generate routes file for network.

To create routeDistributions, run `python createDistributions.py`.

Finally, copy contents of `<routes>` from `routeDist.xml` into the corresponding root of `osm_pt.rou.xml`.
