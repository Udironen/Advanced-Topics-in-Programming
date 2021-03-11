#include "gtest/gtest.h"
#include "Grid.h"
#include "CoordinatesMath.h"
#include <cassert>

#include <list>
#include <memory>
#include <cassert>
#include <iostream>

TEST(GISEx4,constructorTest){
Grid<int,5> grid;
grid.print();
}

TEST(GISEx4,AuxiliaryTest){
    Grid<int,5> grid;
    EXPECT_EQ ( grid.numRows() , 5ul );
    EXPECT_EQ ( grid.numCells() , 32ul );

    EXPECT_EQ ( grid.numCols(Coordinates(Longitude(72),Latitude(18))) , 10ul );
}

TEST(GISEx4,limitsTest){
Grid<std::size_t,5> grid;
auto coords1 = Coordinates(Longitude(135),Latitude(54));
auto coords2 = Coordinates(Longitude(135),Latitude(53));
auto coords3 = Coordinates(Longitude(136),Latitude(54));
auto coords4 = Coordinates(Longitude(136),Latitude(53));

auto coords5 = Coordinates(Longitude(134),Latitude(54));
auto coords6 = Coordinates(Longitude(135),Latitude(55));
auto coords7 = Coordinates(Longitude(134),Latitude(55));
std::size_t i = 0;
const auto cell_ptr = grid.getCellAt(coords1);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords1,++i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords2,++i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords3,++i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords4,++i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords5,i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords6,i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
grid.add(coords7,i);
EXPECT_EQ ( cell_ptr->numEntities<std::size_t>() , i );
auto coords8 = Coordinates(Longitude(140),Latitude(50));
const auto cell_ptr2 = grid.getCellAt(coords8);
EXPECT_EQ ( cell_ptr2->numEntities<std::size_t>() , std::size_t(4) );

}

TEST(GISEx4,testPrimitiveType)
{
    Grid<int,10> grid;
    std::list<int> values = {7, 9};
    auto coords = Coordinates(Longitude(70),Latitude(70));
    for(auto& item: values) {
        grid.add(coords,item);
    }
    const auto cell_ptr = grid.getCellAt(coords);
    EXPECT_EQ ( cell_ptr->numEntities<int>() , std::size_t(2) );
    auto view = cell_ptr->getEntitiesView<int>();
    auto itr = values.begin();
    for(auto view_item: view) {
        EXPECT_EQ ( *view_item , *itr );
        EXPECT_EQ ( view_item , &(*itr) );
        ++itr;
    }
    EXPECT_EQ ( view.size() , std::size_t(2) );
    int i = 8;
    grid.add(coords,i);
    EXPECT_EQ ( cell_ptr->numEntities<int>() , std::size_t(3) );
    EXPECT_EQ ( view.size() , std::size_t(3) );
}

TEST(GISEx4,testPolymorphic)
{
    struct A {
        virtual ~A() {}
        virtual void foo() const = 0;
    };
    struct B : A { void foo() const override {} };
    struct C : A { void foo() const override {} };
    struct D : A { void foo() const override {} };

    std::list<std::unique_ptr<A>> values;
    values.emplace_back(std::make_unique<B>());
    values.emplace_back(std::make_unique<B>());
    values.emplace_back(std::make_unique<C>());
    Grid<A,10> grid;
    auto coords = Coordinates(Longitude(70),Latitude(70));
    for(auto& item: values) {
        grid.add(coords,*item);
    }
    const auto c = grid.getCellAt(coords);
    EXPECT_EQ ( c->numEntities<B>() , std::size_t(2) );
    EXPECT_EQ ( c->numEntities<C>() , std::size_t(1) );
    EXPECT_EQ ( c->numEntities<D>() , std::size_t(0) );
    auto viewD = c->getEntitiesView<D>();
    EXPECT_EQ ( viewD.size() , std::size_t(0) );
    auto viewC = c->getEntitiesView<C>();
    EXPECT_EQ ( viewC.size() , std::size_t(1) );
    D d;
    grid.add(coords,d);
    EXPECT_EQ ( viewD.size() , std::size_t(1) );
    for(D* pd: viewD) {
    EXPECT_EQ ( pd , &d );
    }
    C cObj;
    grid.add(coords,cObj);
    EXPECT_EQ ( viewC.size() , std::size_t(2) );
    auto viewB = c->getEntitiesView<B>();
    EXPECT_EQ ( viewB.size() , std::size_t(2) );
}

TEST(GISEx4,testNonPolymorphic)
{
    struct A {};
    struct B : A {};
    struct C : A {};
    struct D : A {};

    Grid<A,10> grid;
    auto coords = Coordinates(Longitude(70),Latitude(70));

    const auto c = grid.getCellAt(coords);
    auto viewA = c->getEntitiesView<A>(); // good: can ask for a view on A, as A is not abstract!
    EXPECT_EQ ( viewA.size() , std::size_t(0) );
    B b1, b2;
    grid.add(coords,b1);
    grid.add(coords,b2);
    EXPECT_EQ ( c->numEntities<B>() , std::size_t(2) );
    C cObj;
    grid.add(coords, cObj);
    EXPECT_EQ ( c->numEntities<C>() , std::size_t(1) );
    EXPECT_EQ ( c->numEntities<D>() , std::size_t(0) );
    auto viewD = c->getEntitiesView<D>();
    EXPECT_EQ ( viewD.size() , std::size_t(0) );
    auto viewC = c->getEntitiesView<C>();
    EXPECT_EQ ( viewC.size() , std::size_t(1) );
    D d;
    grid.add(coords, d);
    EXPECT_EQ ( viewD.size() , std::size_t(1) );
    for(D* pd: viewD) {
        EXPECT_EQ ( pd , &d );
    }
    C cObj2;
    grid.add(coords, cObj2);
    EXPECT_EQ ( viewC.size() , std::size_t(2) );
    auto viewB = c->getEntitiesView<B>();
    EXPECT_EQ ( viewB.size() , std::size_t(2) );
}


TEST(GISEx4, getEntitiesTest) {
    struct A {
        virtual ~A() {}
        virtual void foo() const = 0;

    };
    struct B : A { void foo() const override {} };
    struct C : A {
        void foo() const override {}
        static bool isC(const A& a){
            return (typeid(a) == typeid(C));
        }
    };
    struct D : C {
        void foodie() const {}
        static bool isDorC(const A& a){
            return (typeid(a) == typeid(D)) || (typeid(a) == typeid(C));
        }
    };

    Coordinates coord{Longitude{20}, Latitude{30}};
    Grid<A, 10> grid;
    const auto cell_ptr = grid.getCellAt(coord);
    auto viewD = cell_ptr->getEntitiesView<D>(); // O(1), always, even for size == N
    EXPECT_EQ(viewD.size(), 0ul); // assume passed successfully

    // adding a D object to the Grid, such that it should be added to the cell at coord
    C cees[4];
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, cees[i]), cell_ptr);
    }
    std::vector<A*> vecA1 = cell_ptr->getEntities(C::isC);
    EXPECT_EQ(vecA1.size(),4ul);
    for(std::size_t i = 0; i < vecA1.size();++i){
        EXPECT_EQ(vecA1[i],&(cees[i]));
    }

    D dees[4];
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, dees[i]), cell_ptr);
    }
    std::vector<A*> vecA2 = cell_ptr->getEntities(D::isDorC);
    EXPECT_EQ(vecA2.size(),8ul);

    B bees[4];
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, bees[i]), cell_ptr);
    }

    std::vector<A*> vecA3 = cell_ptr->getEntities(D::isDorC);
    EXPECT_EQ(vecA3.size(),8ul);

    EXPECT_EQ(cell_ptr->numEntities(), 12ul);
    EXPECT_EQ(cell_ptr->numEntities<C>(), 4ul);

}


TEST(GISEx4, getEntitiesActualTTest) {
    struct A {
        virtual ~A() {}
        virtual void foo() const = 0;
    };
    struct B : A {
        void foo() const override {}
        static bool isB(const B& b){
            return (typeid(b) == typeid(B));
        }
    };
    struct C : A {
        void foo() const override {}
        static bool isC(const A& a){
            return (typeid(a) == typeid(C));
        }
    };
    struct D : C {
        void foodie() const {}
        static bool isDorC(const A& a){
            return (typeid(a) == typeid(C)) || (typeid(a) == typeid(C));
        }

    };

    Coordinates coord{Longitude{20}, Latitude{30}};
    Grid<A, 10> grid;
    const auto cell_ptr = grid.getCellAt(coord);
    auto viewD = cell_ptr->getEntitiesView<D>(); // O(1), always, even for size == N
    EXPECT_EQ(viewD.size(), 0ul); // assume passed successfully

    // adding a D object to the Grid, such that it should be added to the cell at coord
    C cees[4];
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, cees[i]), cell_ptr);
    }
    std::vector<C*> vecC1 = cell_ptr->getEntities<C>(C::isC);
    EXPECT_EQ(vecC1.size(),4ul);
    for(std::size_t i = 0; i < vecC1.size();++i){
        EXPECT_EQ(vecC1[i],&(cees[i]));
    }

    std::vector<D*> vecD1 = cell_ptr->getEntities<D>(C::isC);
    EXPECT_EQ(vecD1.size(),0ul);

    B bees[4];
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, bees[i]), cell_ptr);
    }
    std::vector<B*> vecB1 = cell_ptr->getEntities<B>(C::isC);
    EXPECT_EQ(vecB1.size(),0ul);

    D dees[4];
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, dees[i]), cell_ptr);
    }
    std::vector<D*> vecD2 = cell_ptr->getEntities<D>(C::isC);
    EXPECT_EQ(vecD2.size(),0ul);

    std::vector<C*> vecC2 = cell_ptr->getEntities<C>(C::isC, 2ul);
    EXPECT_EQ(vecC2.size(),2ul);
    for(std::size_t i = 0; i < vecC2.size();++i){
        EXPECT_EQ(vecC2[i],&(cees[i]));
    }

    std::vector<B*> vecB2 = cell_ptr->getEntities<B>(B::isB);
    EXPECT_EQ(vecB2.size(),4ul);
    for(std::size_t i = 0; i < vecB2.size();++i){
        EXPECT_EQ(vecB2[i],&(bees[i]));
    }
}

TEST(GISEx4, EntityIteratorTest){
    struct A {
        virtual ~A() {}
        virtual void foo() const = 0;
        virtual bool notConst() {return 0==0;}
    };
    struct B : A {
        void foo() const override {}
    };
    struct C : A {
        void foo() const override {}
        static bool isC(const A& a){
            return (typeid(a) == typeid(C));
        }
    };
    struct D : C {
        void foodie() const {}
        static bool isDorC(const A& a){
            return (typeid(a) == typeid(C)) || (typeid(a) == typeid(C));
        }

    };


    Coordinates coord{Longitude{20}, Latitude{30}};
    Grid<A, 10> grid;
    const auto cell_ptr = grid.getCellAt(coord);
    C cees[4];
    for(int i=0; i<4; ++i) {
    EXPECT_EQ(grid.add(coord, cees[i]), cell_ptr);
    }
    B bees[4];
    for(int i=0; i<4; ++i) {
    EXPECT_EQ(grid.add(coord, bees[i]), cell_ptr);
    }
    D dees[4];
    for(int i=0; i<4; ++i) {
    EXPECT_EQ(grid.add(coord, dees[i]), cell_ptr);
    }
    auto itr = cell_ptr->begin();
    auto end_itr = cell_ptr->end();
    std::size_t cnt = 0;
    std::size_t cnt2 = 0;
    while(itr != end_itr){
        if(typeid(*(*itr)) == typeid(C)){
            EXPECT_EQ(*itr,&(cees[cnt2]));
            cnt2++;
            EXPECT_TRUE((*itr)->notConst());
        }
        ++itr;
        ++cnt;
    }
    EXPECT_EQ(cnt,12ul);
    EXPECT_EQ(cnt2,4ul);
}


TEST(GISEx4,CellIteratorTest){
    Grid<int,5> grid;
    EXPECT_EQ ( grid.numCells() , 32ul );
    auto itr = grid.begin();
    auto itr_end = grid.end();
    std::size_t cnt = 0;
    while(itr!=itr_end){
        ++itr;
        ++cnt;
    }
    EXPECT_EQ ( cnt , 32ul );
}

TEST(GISEx4, DocTest) {
    struct A {
        virtual ~A() {}
        virtual void foo() const = 0;
    };
    struct B : A { void foo() const override {} };
    struct C : A { void foo() const override {} };
    struct D : C { void foodie() const {} };

    Coordinates coord{Longitude{20}, Latitude{30}};
    Grid<A, 1917> grid;
    const auto cell_ptr = grid.getCellAt(coord);
    auto viewD = cell_ptr->getEntitiesView<D>(); // O(1), always, even for size == N
    EXPECT_EQ(viewD.size(), 0ul); // assume passed successfully

    // adding a D object to the Grid, such that it should be added to the cell at coord
    D dees[4];
    const auto cell_ptr2 = grid.add(coord, dees[0]);
    EXPECT_EQ(viewD.size(), 1ul); // the new object shall be in the view
    EXPECT_EQ(cell_ptr2, cell_ptr);

    // adding 3 D objects to the Grid, such that it should be added to the cell at coord
    for(int i=1; i<4; ++i) {
        EXPECT_EQ(grid.add(coord, dees[i]), cell_ptr);
    }

    EXPECT_EQ(viewD.size(), 4ul); // the new objects shall be in the view

    int i=0;
    for(D* pd: viewD) { // correct type
        pd->foodie();
        EXPECT_EQ(pd, &dees[i]);
        ++i;
    }
}
