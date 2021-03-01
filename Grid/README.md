# Grid-ex4

In this exercise you are required to manage a “Spherical Grid” class holding data according to Geo-location, based on the following requirements:
-	The “Spherical Grid” is aimed to manage data that is owned by other data structures. This is also how you would have to test it.
-	The actual data is of an unknown type (modeled as a Template parameter) and should support polymorphism, that is if the template parameter passed to the Grid is “Entity” then it shall support managing “Entity” and any class which is publicly derived from “Entity”.
-	Geo-location shall be managed based on a Coordinates class similar to the one used in previous exercises. You will get this class as part of ex4-skeleton.
-	Geo-location calculations shall be based on a CoordinatesMath class similar to the one used in previous exercises. You will get this class as part of ex4-skeleton.
-	The Grid model is described below, it would be similar (though, not necessarily exactly as) “Kurihara reduced grid” (see original paper from 1965, don’t feel obliged to read it).
-	You may manage the Grid as a “sparse grid” or not (i.e. it is perfectly OK to create all Cells ahead, but it is also OK to create the cells as needed). A public static constexpr bool variable named is_sparse inside class Grid would indicate your decision.
-	You are required to implement the exact API described below

 
Grid Cells

●	All cells are of the same “height” (“distance” between cell’s bottom latitude and cell’s top latitude is the same for all cells).
●	Cells have different “width” (“distance” between cell’s left longitude and cell’s right longitude) - according to their distance from the equator. Cells which are closer to the equator would have smaller “width” (more cells at this latitude). Cells which are closer to the poles would have bigger “width” (less cells at this latitude).
●	The “first” cell for each given latitude (“row”) would always have its “left” longitude exactly on zero (“Greenwich line”). And the “last” cell on the “row” would always have its “right” longitude exactly on zero. 
●	Number of “rows” in the grid is provided to the Grid class as a template parameter - see below. Note that the equator can be between rows or exactly in the middle of the  middle row of the grid, depending on whether the number of rows is even or odd.
●	Number of “cols” in the grid, per a given “row” is calculated so that the actual area size of all cells would be approximately the same. This is done by calculating the average distance along the top latitude and the bottom latitude on the entire sphere, i.e.: (top_latitude_circle_perimeter + bottom_latitude_circle_perimeter) / 2, then dividing it by the actual “height” measurement of the cell, and taking the ceiling value.

Grid’s public API

template<typename Entity, std::size_t num_rows> requires (num_rows > 0)
class Grid { ... };

Grid’s Constructors and Assignment:
There are no constructors that are required by the API. You should decide whether to implement, block or rely on the default for copy and move constructors, as well as for the assignment operators.

Grid’s Destructor:
You should decide whether there is a need to implement a user defined destructor.

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
 
additional auxiliary functions:
(Grid::C1)	std::size_t numRows() const noexcept; 
(Grid::C2)	std::size_t numCols(Coordinates c) const noexcept; 
(Grid::C3)	std::size_t numCells() const noexcept; 

iterators begin and end:
The Grid would have a const version of begin and end iterators for iterating over all Cells. There is no defined order for the iteration. The iteration itself shall not create any copies. Dereferencing the iterator shall return const Cell&.
If you are implementing a sparse grid, iterating should not return empty cells.
If you are implementing a non-sparse grid, iterating should return all cells.

Cell’s public API

Cell shall be a nested private* class inside class Grid.
*	C++ allows calling public methods on an object obtained legally, even if the class itself is defined in some private scope.

class Cell { ... };

Cell’s Constructors and Assignment:
There are no constructors that are required by the API. You should decide whether to implement, block or rely on the default for copy and move constructors, as well as for the assignment operators.

Cell’s Destructor:
You should decide whether there is a need to implement a user defined destructor.

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
(1)	Complexity of this function is required to be O(1)
(2)	This function returns a view that is updated “behind the scene” automatically in case additional objects of type ActualT are added to this Cell via the Grid. Order of entities in the view shall be the same as the order of their insertion to the grid.
(3)	Iterating over the returned view generates pointers with the correct type, i.e. ActualT*.

 
Code example:
const Cell* cell = getCellAt(coord); // assume ‘coord’ was defined already above
auto viewD = cell->getEntitiesView<D>(); // O(1), always, even for size == N
assert ( viewD.size() == 0 ); // assume passed successfully
// adding a D object to the Grid, such that it should be added to the cell at coord
assert ( viewD.size() == 1 ); // the new object shall be in the view
// adding three additional D objects via the Grid, that should be added to this cell
assert ( viewD.size() == 4 ); // the new objects shall be added at the end
for(D* pd: viewD) { // correct type
  pd->call_a_function_appearing_only_in_class_D_without_casting();
}

Additional auxiliary functions:
(Cell::C1)
std::size_t numEntities() const noexcept;

(Cell::C2)
template<typename ActualT>
requires concrete_derived_or_same<ActualT, Entity>
std::size_t numEntities() const noexcept; // NOTE: complexity required to be O(1)

Iterators begin and end:
The Cell would have begin and end iterators for iterating over all pointers to Entity. Retrieved pointers are non-const, i.e. the user can call non-const methods on the retrieved entities. There is no defined order for the iteration. Iteration itself shall not create any copies.
Dereferencing the iterator shall return Entity*.
________________________________________

Tests:
You are required to implement and submit googletests that demonstrate how you tested your code.

________________________________________

Additional submission instructions:
TBD

________________________________________

Colors Legend:

●	There is one function colored in yellow which is considered a bonus part - you can waive it and still get 100.

________________________________________

FAQ

Q1: When given coordinates are exactly on the border between two cells, for which cell should they belong?
A: To the cell on the right / bottom, i.e. cell holds: [Left, Right), [Top, Bottom)

Q2: When returning a vector<Entity*>, is there a defined order?
A: For returning known objects (e.g. of type ActualT) there is a defined order: as the order of their insertion. For returning ALL objects (of all types) there isn’t any defined order.

 
Q3: What is the signature of the Predicate function?
A: Here it is:
	For unknown generic type calls:
		bool pred(const Entity&);
For specific known type ActualT, both the above is legal, as well as the below:
		bool pred(const ActualT&);
	You should make sure that your code works well if either of the options is used.

Q4: There was a discussion in class regarding the need for the Grid’s add methods to be templated, e.g.:
	template<typename ActualT> requires derived_or_same<ActualT, Entity>
	const Cell* add(Coordinates c, ActualT& e);
It seems that they are still templated, is there a need for that? If yes - why?
A: After re-examining it, there is still a need for these methods to be templated, to allow non-polymorphic class type with inheritance hierarchy to be admitted according to their actual type and use typeid correctly in compile-time, as seen in the test “testNonPolymorphic” at this code example: http://coliru.stacked-crooked.com/a/fb5386db798887d6 (getting the parameter to add as mere Entity& in that case would have get the wrong typeid, which is calculated at compile time for non-polymorphic types).

Q5: What happened to some functions that appeared in the original document? Many of them are “gone”!! Does it mean that we do not need to implement them?
A: After re-examining it, we decided to further narrow the exercise requirements. We removed some of the functions, including the bonus function. A previously non bonus function is now set as bonus. The result is that the requirements are indeed reduced.
________________________________________


Good Luck!
