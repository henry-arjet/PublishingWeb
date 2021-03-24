import React, { Component } from 'react';
import Card from 'react-bootstrap/Card'
import { LinkContainer } from 'react-router-bootstrap';
import { Container, Col, Row } from 'react-bootstrap';

function CategoryCard(props){
    return (
        <Card className="storyCard">
            <LinkContainer to = {"/" + props.link}>
                <Card.Body>
                    <Card.Title>{props.title}</Card.Title>
                    <Card.Text>{props.description}</Card.Text>
                </Card.Body>
            </LinkContainer>
        </Card>
    )
}
        

export default CategoryCard;