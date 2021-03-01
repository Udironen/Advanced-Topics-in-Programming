# Grid-ex4

Inprogram manages a “Spherical Grid” class holding data according to Geo-location, based on the following requirements:
-	The “Spherical Grid” is aimed to manage data that is owned by other data structures.
-	The actual data is of an unknown type (modeled as a Template parameter) and should support polymorphism, that is if the template parameter passed to the Grid is “Entity” then it shall support managing “Entity” and any class which is publicly derived from “Entity”.
-	Geo-location shall be managed based on a Coordinates class.
-	Geo-location calculations shall be based on a CoordinatesMath class.
-	The Grid model is described below, it would be similar (though, not necessarily exactly as) “Kurihara reduced grid” (see original paper from 1965).

Grid Cells
-

?	All cells are of the same “height” (“distance” between cell’s bottom latitude and cell’s top latitude is the same for all cells).
?	Cells have different “width” (“distance” between cell’s left longitude and cell’s right longitude) - according to their distance from the equator. Cells which are closer to the equator would have smaller “width” (more cells at this latitude). Cells which are closer to the poles would have bigger “width” (less cells at this latitude).
?	The “first” cell for each given latitude (“row”) would always have its “left” longitude exactly on zero (“Greenwich line”). And the “last” cell on the “row” would always have its “right” longitude exactly on zero. 
?	Number of “rows” in the grid is provided to the Grid class as a template parameter - see below. Note that the equator can be between rows or exactly in the middle of the  middle row of the grid, depending on whether the number of rows is even or odd.
?	Number of “cols” in the grid, per a given “row” is calculated so that the actual area size of all cells would be approximately the same. 

Grid’s public API
-

template<typename Entity, std::size_t num_rows> requires (num_rows > 0)
class Grid { ... };

Adding Entities to the Grid:
________________________________________
Two concepts required by the below functions:

  template<class Me, class Other> concept derived_or_same =
    std::same_as<Me, Other> || std::derived_from<Me, Other>;

  template<class Me, class Other> concept concrete_derived_or_same =
    !std::is_abstract_v<Me> && derived_or_same<Me, Other>;
________________________________________
(Grid::A1)  template<typename ActualT> requires derived_or_same<ActualT, Entity>
		const Cell* add(Coordinates c, ActualT& e);

Getting Cells from the Grid:
(Grid::B1)  const Cell* getCellAt(Coordinates c) const;

(Grid::B2)  std::vector<const Cell*> getCellsAt(Coordinates center, Meters radius) const;
Notes: there is no defined order for the returned cells. Same cell shall not appear twice of course. 
?
additional auxiliary functions:
(Grid::C1)	std::size_t numRows() const noexcept; 
(Grid::C2)	std::size_t numCols(Coordinates c) const noexcept; 
(Grid::C3)	std::size_t numCells() const noexcept; 

iterators begin and end:
The Grid would have a const version of begin and end iterators for iterating over all Cells. There is no defined order for the iteration. The iteration itself does not create any copies. Dereferencing the iterator shall return const Cell&.
Iiterating will return all cells.

The iterators are implemented by another helper-class.
Since the grid is not sparce, the iterator iterates over all Cells.

Cell’s public API
-

Cell shall be a nested private* class inside class Grid.

class Cell { ... };

Getting Entities from a Cell:
(Cell::A1) template<typename PredicateFunc>
 	 std::vector<Entity*> getEntities(PredicateFunc&&) const;

returns all entities in that cell, which comply with the PredicateFunc, with no defined order. 

Getting specific type of Entities from a Cell:
(Cell::B1) template<typename ActualT, typename PredicateFunc>
 requires concrete_derived_or_same<ActualT, Entity>
 std::vector<ActualT*> getEntities(PredicateFunc&&) const;

returns all entities of type ActualT which comply with the PredicateFunc, in the order of their insertion.

(Cell::B2) template<typename ActualT, typename PredicateFunc>
 requires concrete_derived_or_same<ActualT, Entity>
	 std::vector<ActualT*> getEntities(PredicateFunc&&, std::size_t limit) const;

same as (Cell::B1) above but with a limit on the number of returned entities (up to limit entities).

(Cell::B3) template<typename ActualT>
 requires concrete_derived_or_same<ActualT, Entity>
	 std::ranges::sized_range auto getEntitiesView() const;

returns a range of all entities of type ActualT.

NOTEs:
(1)	Complexity of this function is O(1)
(2)	This function returns a view that is updated “behind the scene” automatically in case additional objects of type ActualT are added to this Cell via the Grid. Order of entities in the view will be the same as the order of their insertion to the grid.
(3)	Iterating over the returned view generates pointers with the correct type, i.e. ActualT*.

Additional auxiliary functions:
(Cell::C1)
std::size_t numEntities() const noexcept;

(Cell::C2)
template<typename ActualT>
requires concrete_derived_or_same<ActualT, Entity>
std::size_t numEntities() const noexcept; // NOTE: complexity required to be O(1)

Iterators begin and end:
The Cell has begin and end iterators for iterating over all pointers to Entity. Retrieved pointers are non-const, i.e. the user can call non-const methods on the retrieved entities. There is no defined order for the iteration. Iteration itself will not create any copies.
Dereferencing the iterator will return Entity*.

Implemented by another field:
        std::vector<Entity*> entities;

________________________________________

Tests:
There are multiple tests in tests.cpp file
________________________________________
