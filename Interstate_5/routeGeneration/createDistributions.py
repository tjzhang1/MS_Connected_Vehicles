import sys
import sumolib

exit_probabilities = {
    "I605": 21.98,
    "LAKEWOOD": 3.47,
    "PARAMOUNT_BLVD": 3.85,
    "SLAUSON_AVE": 5.34,
    "GARFIELD_AVE": 4.57,
    "WASHINGTON_BLVD": 4.69,
    "ATLANTIC_BLVD": 2.10,
    "ATLANTIC_TRIGGS": 1.60,
    "EASTERN_AVE": 1.29,
    "I710": 11.11,
    "DITMAN_AVE": 2.27,
    "CALZONA_ST": 0.40,
}


with open("routeDist.xml", 'w' ) as outf:
    attrs = {'route': ['id','edges']}
    routes = list(sumolib.xml.parse('source_routes.xml', 'routes', attrs))[0]
    source_names = []
    # Rename route IDs
    for route in routes.getChild('route'):
        edges = route.edges.split(" ")
        source = edges[0].split("source_")[-1]  # drop prefix
        dest = edges[-1].split("exit_to_")[-1]  # drop prefix
        name = "route_" + source + "_to_" + dest
        route.id = name
        if source not in source_names:
            source_names.append(source)

    # Create route distributions
    for source in source_names:
        dist = routes.addChild("routeDistribution", {"id": "routeDist_"+source})
        last_probability = 100.0
        last_route_id = ""
        for route in routes.getChild('route'):
            first_edge = route.edges.split(" ")[0].split("source_")[-1]
            last_edge = route.edges.split(" ")[-1].split("exit_to_")[-1]
            if first_edge == source: 
                if last_edge == "destination_edge":
                    last_route_id = route.id
                else:
                    r = dist.addChild("route", {"refId": route.id, 'probability': "%0.1f" % (100.0*exit_probabilities[last_edge])} )
                    last_probability -= exit_probabilities[last_edge]
        dist.addChild("route", {"refId": last_route_id, "probability": "%0.1f" % (100.0*last_probability)} )
            
    outf.write(routes.toXML())
