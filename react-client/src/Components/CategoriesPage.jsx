import React, { Component } from 'react';
import CardGrid from './Cards/CardGrid'

function CategoriesPage() {
    let cats = [{name: "test", linkName:"test", description: "a test category"},{name: "test 2", linkName:"test_2", description: "a second test category"}];
    return(<CardGrid categories={cats} />);
}

export default CategoriesPage;