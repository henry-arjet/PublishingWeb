import React, { Component } from 'react';
import CardGrid from './Cards/CardGrid'

function CategoriesPage() {
    let cats = [{name: "Science Fiction", linkName:"scifi", description: "bruh you know what scifi is", mask: 0b1},
        {name: "test 2", linkName:"test_2", description: "a second test category", mask: 0b10}];
    return(<CardGrid categories={cats} />);
}

export default CategoriesPage;