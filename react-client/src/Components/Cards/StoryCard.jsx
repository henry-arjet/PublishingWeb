import React, { Component } from 'react';
import Card from 'react-bootstrap/Card'
import { LinkContainer } from 'react-router-bootstrap';

class StoryCard extends Component {
    constructor(props){
        super(props);
        this.state = {
            cardArray: null,
        }
    }
    render() {
        return (
            <Card className="storyCard">
                <LinkContainer to = {this.props.link}>
                    <Card.Body>
                        <Card.Title>{this.props.title}</Card.Title>
                        <Card.Text>{this.props.text}</Card.Text>
                    </Card.Body>
                </LinkContainer>
            </Card>
        )
    }
}

export default StoryCard;