#include "Navigation.h"

REGISTER_NAVIGATION(Navigation_313452542)

Navigation_313452542::Navigation_313452542(const NavigationGIS& gis_) : AbstractNavigation(gis_) {}

void Navigation_313452542::constructPath(
    std::vector<std::pair<EntityId, Direction>>& all_ways, const Node& goal) const {
    std::shared_ptr<Node> current_node = std::make_shared<Node>(goal);
    while (((std::string)(current_node->getWayFromParentId())).compare((std::string)"") != 0) {
        all_ways.push_back({ current_node->getWayFromParentId(), current_node->getDirection() });
        current_node = current_node->getParent();
  
    }
    std::reverse(std::begin(all_ways), std::end(all_ways));
}

void Navigation_313452542::resetNode(Node& node)const {
    node.setCostLength(Meters(1e9));
    node.setCostTime(Minutes(1e9));
}

Direction Navigation_313452542::getDirection(
    const std::pair<EntityId, EntityId>& from_to, const EntityId& junc_id) const {

    if (((std::string)from_to.first).compare((std::string)junc_id) == 0) {
        return Direction::A_to_B;
    }
    return Direction::B_to_A;
}

const EntityId& Navigation_313452542::getNeighborId(
    const std::pair<EntityId, EntityId>& from_to, const Direction direction) const {

    switch (direction)
    {
    case Direction::A_to_B:
        return from_to.second;
    default:
        return from_to.first;
    }
}

Coordinates Navigation_313452542::getNeighborCoords(
    const AbstractWay& way, const Direction direction) const {

    switch (direction)
    {
    case Direction::A_to_B:
        return way.getToJunctionCoordinates();
    default:
        return way.getFromJunctionCoordinates();
    }
}

void Navigation_313452542::updateNeighbors(const Node& node,
    std::unordered_map<EntityId, Node, IDhash>& seen_nodes,
    std::unordered_map<EntityId, const AbstractWay&, IDhash>& seen_ways,
    PriorityQueue<iter_node, double>& frontier,
    const std::pair<Meters, Meters>& dist_from_to,
    const Node& goal, const bool is_time) const {

    std::vector<EntityId> ways_id = gis.getWaysByJunction(node.getId());
    Minutes neighbor_cost_time = Minutes(0);
    Meters neighbor_cost_length = Meters(0);
    Meters length = Meters(0);
    Minutes time = Minutes(0);
    EntityId neighbor_id(EntityId(""));
    Coordinates coords = Coordinates(Longitude(Node::defult_longi), Latitude(Node::defult_lati));
    Meters dist_to_goal = Meters(0);
    for (const auto& id : ways_id) {
        auto itr_way = seen_ways.find(id);
        if (itr_way == seen_ways.end()) {
            itr_way = seen_ways.insert({ id, gis.getWay(id) }).first;
        }
        const AbstractWay& way = itr_way->second;
        auto from_to_id = way.getJunctions();
        Direction direction = getDirection(from_to_id, node.getId());

        if (((std::string)goal.getWayFromParentId()).compare((std::string)id) == 0) {
            neighbor_id = goal.getId();
            switch (direction)
            {
            case Direction::B_to_A:
                length = dist_from_to.second;
                break;

            default:
                length = dist_from_to.first;
                break;
            }
            coords = goal.getCoordinates();
        }
        else {
            neighbor_id = getNeighborId(from_to_id, direction);
            length = way.getLength();

            coords = getNeighborCoords(way, direction);
        }
        time = Node::calcTime(length, way.getSpeedLimit());
        neighbor_cost_time = Minutes((double)node.getCostTime() + (double)time);
        neighbor_cost_length = Meters((double)node.getCostLength() + (double)length);
    
        auto itr = seen_nodes.find(neighbor_id);
        if (itr != seen_nodes.end()) {
            bool is_update = false;
            if (is_time) {
                if ((double)neighbor_cost_time < (double)itr->second.getCostTime()) {
                    is_update = true;
                }

                else if ((double)neighbor_cost_time == (double)itr->second.getCostTime()) {
                    if ((double)neighbor_cost_length < (double)itr->second.getCostLength()) {
                        is_update = true;
                    }
                    else if ((double)neighbor_cost_length == (double)itr->second.getCostLength()) {
                        if (node.getNumOfWay() + 1 < itr->second.getNumOfWay()) {
                            is_update = true;
                        }
                    }
                }

            }
            else {
                if ((double)neighbor_cost_length < (double)itr->second.getCostLength()) {
                    is_update = true;
                }
                else if ((double)neighbor_cost_length == (double)itr->second.getCostLength()) {
                    if ((double)neighbor_cost_time < (double)itr->second.getCostTime()) {
                        is_update = true;
                    }
                    else if ((double)neighbor_cost_time == (double)itr->second.getCostTime()) {
                        if (node.getNumOfWay() + 1 < itr->second.getNumOfWay()) {
                            is_update = true;
                        }
                    }
                }
            }
            if (is_update) {

                itr->second.setCostLength(neighbor_cost_length);
                itr->second.setCostTime(neighbor_cost_time);
                itr->second.setDirection(direction);
                itr->second.setParent(node);
                itr->second.setWayFromParentId(id);
                itr->second.setNumOfWay((node.getNumOfWay() + 1));
                frontier.put(itr, itr->second.getPriority(is_time));
            }
        }
        else {
            dist_to_goal = CoordinatesMath::calculateDistance(coords, goal.getCoordinates());
            auto itr_neighbor = seen_nodes.insert({ neighbor_id ,
                Node(neighbor_id, node, id, direction,
                neighbor_cost_length,
                neighbor_cost_time,
                (double)dist_to_goal,
                coords) }).first;
            frontier.put(itr_neighbor, itr_neighbor->second.getPriority(is_time));
        }
    }
}

bool Navigation_313452542::aStarSearch(Node& goal,
    std::unordered_map<EntityId, Node, IDhash>& seen_nodes,
    std::unordered_map<EntityId, const AbstractWay&, IDhash>& seen_ways,
    PriorityQueue<iter_node, double>& frontier,
    const std::pair<Meters, Meters>& dist_from_to, const bool is_time) const {

    while (!frontier.empty()) {
        iter_node min_node = frontier.get();
        if (((std::string)min_node->first).compare((std::string)(goal.getId())) == 0) {
            goal = min_node->second;
            

            return true;
        }
        updateNeighbors(min_node->second, seen_nodes, seen_ways, frontier, dist_from_to, goal, is_time);
    }
    return false;
}

bool Navigation_313452542::isValidPoint(const AbstractWay& way, const Coordinates& point, const Coordinates& coordinate) const {
    Meters distance = CoordinatesMath::calculateDistance(point, coordinate);
    return !((way.isHighway()) && ((double)distance > 3));
}


// ########### get routes ##################
std::unique_ptr<AbstractRoutes>  Navigation_313452542::getRoutes(const Coordinates& start, const Coordinates& end) const {

    auto start_tuple = gis.getWayClosestPoint(start);
    const AbstractWay* first_way = &(gis.getWay(std::get<1>(start_tuple)));
    bool is_start_valid = isValidPoint(*first_way, std::get<0>(start_tuple), start);
    if (!is_start_valid) {
        Restrictions res = Restrictions("highway");
        start_tuple = gis.getWayClosestPoint(start, res);
        first_way = &(gis.getWay(std::get<1>(start_tuple)));
        is_start_valid = isValidPoint(*first_way, std::get<0>(start_tuple), start);
    }
    auto end_tuple = gis.getWayClosestPoint(end);
    const AbstractWay* last_way = &(gis.getWay(std::get<1>(end_tuple)));
    bool is_end_valid = isValidPoint(*last_way, std::get<0>(end_tuple), end);
    if (!is_end_valid) {
        Restrictions res = Restrictions("highway");
        end_tuple = gis.getWayClosestPoint(end, res);
        last_way = &(gis.getWay(std::get<1>(end_tuple)));
        is_end_valid = isValidPoint(*last_way, std::get<0>(end_tuple), end);
    }

    if ((!is_start_valid) || (!is_end_valid) || isSameWay(start_tuple, end_tuple)) {
        return std::make_unique<Routes>(Route(), Route(), false);;
    }
    auto first_from_to = first_way->getJunctions();

    const Coordinates& from_first_way = first_way->getFromJunctionCoordinates();
    const Coordinates& to_first_way = first_way->getToJunctionCoordinates();
    Direction direction = Direction::A_to_B;
    auto dist_from_to = first_way->getSegmentPartsOnWay(std::get<2>(start_tuple), std::get<0>(start_tuple));

    std::unordered_map<EntityId, Node, IDhash> seen_nodes;
    std::unordered_map<EntityId, Node, IDhash> seen_nodes_time;
    std::unordered_map<EntityId, const AbstractWay&, IDhash> seen_ways;
    seen_ways.insert({ first_way->getId(),*first_way });
    seen_ways.insert({ last_way->getId(),*last_way });

    PriorityQueue<iter_node, double> frontier{};
    PriorityQueue<iter_node, double> frontier_time{};
    Minutes duration = Minutes(0);
    Meters dist = Meters(0);
    double her = (double)CoordinatesMath::calculateDistance(std::get<0>(start_tuple), std::get<0>(end_tuple));

    if (from_first_way == std::get<0>(start_tuple)) {
        Node from_node = Node(first_from_to.first, Node(),
            EntityId(""), direction, dist, duration, her, from_first_way);
        auto itr = seen_nodes.insert({ from_node.getId(),from_node });
        auto itr_time = seen_nodes_time.insert({ from_node.getId(),from_node });
        frontier.put(itr.first, from_node.getPriority());
        frontier_time.put(itr_time.first, from_node.getPriority(true));
    }
    else if (to_first_way == std::get<0>(start_tuple)) {

        Node to_node = Node(first_from_to.second, Node(),
            EntityId(""), direction, dist, duration, her, to_first_way);
        auto itr = seen_nodes.insert({ to_node.getId(),to_node });
        auto itr_time = seen_nodes_time.insert({ to_node.getId(),to_node });
        frontier.put(itr.first, to_node.getPriority());
        frontier_time.put(itr_time.first, to_node.getPriority(true));

    }
    else {
        dist = dist_from_to.second;
        duration = Node::calcTime(dist_from_to.second, first_way->getSpeedLimit());
        Node to_node = Node(first_from_to.second, Node(),
            std::get<1>(start_tuple), direction, dist, duration, her, to_first_way);
        auto itr = seen_nodes.insert({ to_node.getId(),to_node });
        auto itr_time = seen_nodes_time.insert({ to_node.getId(),to_node });
        frontier.put(itr.first, to_node.getPriority());
        frontier_time.put(itr_time.first, to_node.getPriority(true));

        if (first_way->isBidirectional()) {
            dist = dist_from_to.first;
            direction = Direction::B_to_A;
            duration = Node::calcTime(dist_from_to.first, first_way->getSpeedLimit());
            Node from_node = Node(first_from_to.first, Node(),
                std::get<1>(start_tuple), direction, dist, duration, her, from_first_way);
            auto itr = seen_nodes.insert({ from_node.getId(),from_node });
            auto itr_time = seen_nodes_time.insert({ from_node.getId(),from_node });
            frontier.put(itr.first, from_node.getPriority());
            frontier_time.put(itr_time.first, from_node.getPriority(true));
        }

    }

    auto end_from_to = last_way->getJunctions();
    const Coordinates& from_last_way = last_way->getFromJunctionCoordinates();
    const Coordinates& to_last_way = last_way->getToJunctionCoordinates();

    Node goal_node = Node(EntityId(GOAL), Node(), std::get<1>(end_tuple),
        direction, Meters(1e9), Minutes(1e9), 0, std::get<0>(end_tuple));

    if (from_last_way == std::get<0>(end_tuple)) {
        goal_node.setId(last_way->getJunctions().first);
        goal_node.setWayFromParentId(EntityId(""));
    }
    else if (to_last_way == std::get<0>(end_tuple)) {
        goal_node.setId(last_way->getJunctions().second);
        goal_node.setWayFromParentId(EntityId(""));
    }
    seen_nodes.insert({ goal_node.getId(),goal_node });
    seen_nodes_time.insert({ goal_node.getId(),goal_node });
    bool success_search;
    dist_from_to = last_way->getSegmentPartsOnWay(std::get<2>(end_tuple), std::get<0>(end_tuple));
    success_search = aStarSearch(goal_node, seen_nodes, seen_ways, frontier, dist_from_to);
    if (!success_search) {
        return std::make_unique<Routes>(Route(), Route(), false);;
        ;
    }
    std::vector<std::pair<EntityId, Direction>> all_ways;
    constructPath(all_ways, goal_node);
    Route&& shortest_distance = Route(all_ways, std::get<0>(start_tuple), std::get<0>(end_tuple),
        goal_node.getCostLength(), goal_node.getCostTime());

    resetNode(goal_node);
    
    if (from_last_way == std::get<0>(end_tuple)) {
        goal_node.setId(last_way->getJunctions().first);
        goal_node.setWayFromParentId(EntityId(""));
    }
    else if (to_last_way == std::get<0>(end_tuple)) {
        goal_node.setId(last_way->getJunctions().second);
        goal_node.setWayFromParentId(EntityId(""));
    }
    aStarSearch(goal_node, seen_nodes_time, seen_ways, frontier_time, dist_from_to, true);
    all_ways.clear();
    constructPath(all_ways, goal_node);
 
    Route&& shortest_time = Route(all_ways, std::get<0>(start_tuple), std::get<0>(end_tuple),
        goal_node.getCostLength(), goal_node.getCostTime());
    return std::make_unique<Routes>(std::move(shortest_distance), std::move(shortest_time), true);
}


bool Navigation_313452542::isSameWay(
    const std::tuple<Coordinates, EntityId, std::size_t>& start_tuple,
    const std::tuple<Coordinates, EntityId, std::size_t>& end_tuple) const {

    if (((std::string)std::get<1>(start_tuple)).compare((std::string)std::get<1>(end_tuple)) == 0) { return true; }
    return false;

}