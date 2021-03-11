// udironen 313452542

#pragma once

#include "GISNamedTypes.h"
#include "CoordinatesMath.h"
#include <vector>
#include <map>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <concepts>
#include <ranges>
#include <memory>
#include <utility>


// Two concepts required by the below functions:

template<class Me, class Other> concept derived_or_same =
    std::same_as<Me, Other> || std::derived_from<Me, Other>;

template<class Me, class Other> concept concrete_derived_or_same =
	!std::is_abstract_v<Me> && derived_or_same<Me, Other>;

// class Grid
template<typename Entity, std::size_t num_rows> requires (num_rows > 0)
class Grid {

    static constexpr double meridian_180 = 180;
    static constexpr double n_pole = 90;

    // private inner class Cell
    class Cell {
        friend class Grid;
        mutable std::map<std::type_index, std::vector<Entity*>> all_types;
        std::vector<Entity*> entities;

        template<typename ActualT> requires derived_or_same<ActualT, Entity>
        void add(ActualT& e) {
            entities.push_back(&e);
            all_types[typeid(e)].push_back(&e);
        }

        template<typename ActualT, typename PredicateFunc> requires concrete_derived_or_same<ActualT, Entity>
        void getEntities(std::vector<ActualT*>& tees, PredicateFunc&& pred, std::size_t limit) const {
            auto itr = all_types.find(typeid(ActualT));
            if(itr != all_types.end()) {
                for(const auto& t : itr->second){
                    if(tees.size() >= limit) break;
                    if(pred(*(static_cast<ActualT*>(t)))) tees.push_back(static_cast<ActualT*>(t));
                }
            }
        }

    public:
        // Cell’s Constructors and Assignment:

        // Cell’s Destructor:
        // You should decide whether there is a need to implement a user defined destructor.

        // (Cell::A1) Getting Entities from a Cell:
        // returns all entities in that cell, which return true for the PredicateFunc.
        template<typename PredicateFunc>
        std::vector<Entity*> getEntities(PredicateFunc&& pred) const {
            std::vector<Entity*> entities;
            for(const auto& pr : all_types){
                for(const auto& entity : pr.second){
                    if(pred(*entity))
                        entities.push_back(entity);
                }
            }
            return entities;
        }

        // (Cell::B1) 
        // Getting specific type of Entities from a Cell:
        // returns all entities of type ActualT which return true for the PredicateFunc.
        template<typename ActualT, typename PredicateFunc> 
        requires concrete_derived_or_same<ActualT, Entity>
        std::vector<ActualT*> getEntities(PredicateFunc&& pred) const
        {
            std::vector<ActualT*> tees(0);
            getEntities<ActualT>(tees, std::forward<PredicateFunc>(pred), entities.size());
            return tees;
        }

        //  (Cell::B2)
        // same as (Cell::B1) above but with a limit on the number of returned entities (up to limit entities).
        template<typename ActualT, typename PredicateFunc> requires concrete_derived_or_same<ActualT, Entity>
        std::vector<ActualT*> getEntities(PredicateFunc&& pred, std::size_t limit) const
        {
            std::vector<ActualT*> tees;
            getEntities<ActualT>(tees, std::forward<PredicateFunc>(pred), limit);
            return tees;
        }

        // (Cell::B3) 
        // returns a range of all entities of type ActualT.
        // Complexity of this function is required to be O(1)
        // This function returns a view that is updated “behind the scene” automatically in case additional objects of type ActualT are added to this Cell via the Grid. Order of entities in the view shall be the same as the order of their insertion to the grid.
        // Iterating over the returned view generates pointers with the correct type, i.e. ActualT*.
        template<typename ActualT> requires concrete_derived_or_same<ActualT, Entity>
        std::ranges::sized_range auto getEntitiesView() const {
            // helper class
            class view_to_vector_of_pointers {
                std::vector<Entity*>& vec;
            public:
                struct pointers_view_iterator: public std::vector<Entity*>::iterator {
                    pointers_view_iterator() {}
                    pointers_view_iterator(std::vector<Entity*>::iterator itr): std::vector<Entity*>::iterator(itr) {}
                    ActualT* operator*() { return static_cast<ActualT*>(this->std::vector<Entity*>::iterator::operator*()); }
                    const ActualT* operator*() const { return static_cast<const ActualT*>(this->std::vector<Entity*>::iterator::operator*()); }
                    pointers_view_iterator& operator++() {
                        this->std::vector<Entity*>::iterator::operator++();
                        return *this;
                    }
                    pointers_view_iterator operator++(int) {
                        auto old = *this;
                        ++(*this);
                        return old;
                    }
                };
                view_to_vector_of_pointers(std::vector<Entity*>& vec): vec(vec) {}
                auto begin() const { return pointers_view_iterator { vec.begin() }; }
                auto end() const { return pointers_view_iterator { vec.end() }; }
                std::size_t size() const { return vec.size(); }
            };
            // end of inner class vector_of_pointers_view
            //-------------------------------------------------
            // 'all_types' is mutable, empty vector might be created below
            return view_to_vector_of_pointers{ all_types[typeid(ActualT)] };
        }
   
        // Additional auxiliary functions:

        // (Cell::C1) 
        std::size_t numEntities() const noexcept {
            return entities.size();
        }

        // (Cell::C2) 
        // NOTE: complexity required to be O(1)
        template<typename ActualT> requires concrete_derived_or_same<ActualT, Entity>
        std::size_t numEntities() const noexcept {
            auto itr = all_types.find(typeid(ActualT));
            if(itr == all_types.end()) return 0;
            return itr->second.size();
        }

        // Iterators begin and end:
        // The Cell would have begin and end iterators for iterating over all pointers to Entity. Retrieved pointers are non-const, i.e. the user can call non-const methods on the retrieved entities. There is no defined order for the iteration. Iteration itself shall not create any copies.
        auto begin() const noexcept { 
            return entities.begin();
        }

        auto end() const noexcept { 
            return entities.end();
        }
    };

    //=============================================================


    std::vector<std::vector<Cell>> cells;
    std::size_t num_cells;

    Meters cellHeight() const{
        return CoordinatesMath::half_earth_hemisphere * 2 / num_rows;
    }

    double latiPrecision() const{
        return double(meridian_180) / num_rows;
    }

    double longiPrecision(std::size_t num_cols) const{
        return double(2 * meridian_180) / num_cols;
    }

    double truncateDecimalLati(double lati) const
    {
        return n_pole - std::floor((n_pole - lati ) / latiPrecision()) * latiPrecision();
    }

    double truncateDecimalLongi(double longi, Latitude lati) const
    {
        std::size_t num_cols = numCols(lati);
        double longi_precision = longiPrecision(num_cols);
        return std::floor(longi / longi_precision) * longi_precision;
    }

    Coordinates truncateCoordinates( Coordinates coordinates) const
    {
        Latitude lati = Latitude(truncateDecimalLati((double)coordinates.latitude()));
        Longitude longi = Longitude{truncateDecimalLongi( (double)coordinates.longitude(), lati)};
        return	{	longi, lati    };
    }

    Latitude getLati(std::size_t i) const{
        return Latitude(-n_pole + (i+1) * latiPrecision());
    }

    Longitude getlongi(Latitude lati , std::size_t j) const{
        std::size_t num_cols = numCols(lati);
        double longi = j * longiPrecision(num_cols);
        if(longi >= meridian_180)
            longi -= meridian_180 * 2;
        return Longitude(longi);
    }

    Coordinates getCoordinates(std::size_t i, std::size_t j) const{
        Latitude lati = getLati(i);
        return Coordinates(getlongi(lati,j),lati);
    }

    void printCoords(Coordinates coords) const{
        std::cout << "[" << double(coords.longitude()) << ", " << double(coords.latitude()) << "]";
    }


    std::size_t numCols(Latitude top_lati) const noexcept{
        Meters bottom_perimeter = CoordinatesMath::perimeterOnLatitude(Latitude(double(top_lati) - latiPrecision()));
        Meters top_perimeter = CoordinatesMath::perimeterOnLatitude(top_lati);
        Meters average_perimeter = (bottom_perimeter + top_perimeter) / 2;
        double num = average_perimeter / cellHeight();
        std::size_t num_cols = num;
        if(num_cols < num)
            num_cols += 1;
        return num_cols;
    }

    std::pair<std::size_t, std::size_t> getIndexes(Coordinates coords) const {
        coords = truncateCoordinates(coords);
        std::size_t i = (double(coords.latitude())) / latiPrecision();
        double longi = double(coords.longitude());
        if(longi < 0){
            longi = meridian_180 - longi;
        }
        std::size_t j = longi / longiPrecision(numCols(coords.latitude()));
        return { i,j };
    }

public:
    // Grid’s Constructors and Assignment:
    // There are no constructors that are required by the API. You should decide whether to implement, block or rely on the default for copy and move constructors, as well as for the assignment operators.
    Grid() : num_cells(0) {
       for (std::size_t i = 0; i < num_rows; ++i){
           Latitude lati = getLati(i);
           std::size_t num_cols = numCols(lati);
           num_cells += num_cols;
           cells.push_back(std::vector<Cell>(num_cols));
       }
    }

    // Grid’s Destructor:
    // You should decide whether there is a need to implement a user defined destructor.

    // Adding Entities to the Grid:

    // (Grid::A1) 
    template<typename ActualT> requires derived_or_same<ActualT, Entity>
    const Cell* add(Coordinates c, ActualT& e)
    {
        auto i_j = getIndexes(c);
        Cell& cell = cells[i_j.first][i_j.second];
        cell.add(e);
        return &cell;
    }

    // Getting Cells from the Grid:

    // (Grid::B1) 
    const Cell* getCellAt(Coordinates c) const {
        auto i_j = getIndexes(c);
        return &(cells[i_j.first][i_j.second]);
    }



    // (Grid::B2) (BONUS: you may choose not to implemnt this function)
    std::vector<const Cell*> getCellsAt(Coordinates center, Meters radius) const {
        (void) center;
        (void) radius;
        return std::vector<const Cell*>{};
    }

    // additional auxiliary functions:

    // (Grid::C1) 
    std::size_t numRows() const noexcept {
        return num_rows;
    } 
    
    // (Grid::C2) 
    std::size_t numCols(Coordinates c) const noexcept {
        return numCols(c.latitude());
    }

    // (Grid::C3) 
    std::size_t numCells() const noexcept {
        return num_cells;
    }

    // iterators begin and end:
    // The Grid would have a const version of begin and end iterators for iterating over all Cells. There is no defined order for the iteration. The iteration itself shall not create any copies.    
    
    // Set this to true if you are implementing a sparse grid.
    static constexpr bool is_sparse = false;  
 
    // Following would iterate over:
    // 1. Only non empty Cells, if is_sparse==true 
    // 2. All Cells, if is_sparse==false

    class CellIterator{
        friend class Grid;
        std::vector<std::vector<Cell>>::const_iterator vec_iter;
        std::vector<std::vector<Cell>>::const_iterator end_vec_iter;
        std::vector<Cell>::const_iterator cell_iter;


        CellIterator(const Grid& grid):
                vec_iter        (grid.cells.begin()),
                end_vec_iter    (grid.cells.end()),
                cell_iter       (vec_iter->begin())    {}

        CellIterator(const Grid& grid , int):
                vec_iter        (grid.cells.end()),
                end_vec_iter    (grid.cells.end()),
                cell_iter       (grid.cells[grid.cells.size() -1].end())     {}


    public:

        bool operator!=(const CellIterator& other) const{
            return !(*this == other);
        }

        bool operator==(const CellIterator& other) const{
            return (cell_iter == other.cell_iter) && (vec_iter == other.vec_iter) ;
        }

        CellIterator& operator++(){
            if(vec_iter == end_vec_iter) return *this;
            ++cell_iter;
            if(cell_iter == vec_iter->end()){
                vec_iter++;
                if(vec_iter == end_vec_iter) return *this;
                cell_iter = vec_iter->begin();
            }
            return *this;
        }

        CellIterator& operator++(int){
            auto old = *this;
            ++(*this);
            return old;
        }

        const Cell& operator*(){
            return *cell_iter;
        }
    };
    //=======================================================================

    auto begin() const noexcept {
        return CellIterator(*this);
    }

    auto end() const noexcept {
        return CellIterator(*this, -1);
    }

    //for testings
    void print() const{
        for(std::size_t i = num_rows; i > 0; --i){
            std::size_t num_cols = numCols(getLati(i-1));
            for(std::size_t j = 0; j < num_cols; ++j){
                printCoords(getCoordinates(i-1 ,j));
                std::cout << " ";
            }
            std::cout <<"\n\n";
        }
    }
};
