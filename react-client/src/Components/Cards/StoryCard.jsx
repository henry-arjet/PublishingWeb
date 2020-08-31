import React, { Component } from 'react';
import Card from 'react-bootstrap/Card'
import { LinkContainer } from 'react-router-bootstrap';
import { Container, Col, Row } from 'react-bootstrap';

class StoryCard extends Component {
    constructor(props){
        super(props);
        this.state = {
            cardArray: null,
        }
    }
    render() {
        console.log(this.props.rating)

        return (
            <Card className="storyCard">
                <LinkContainer to = {"/" + this.props.link}>
                    <Card.Body>
                        <Card.Title>{this.props.title}</Card.Title>
                        <Container >
                            <Row className="StoryCardStats">
                                <Card.Text as="Col">{this.props.rating != null ? this.props.rating + "/5.0": "Unrated"}</Card.Text>
                                <Col />
                                <Card.Text as="Col">{this.props.views + " views"}</Card.Text>
                            </Row>
                        </Container>
                    </Card.Body>
                </LinkContainer>
            </Card>
        )
    }
}

export default StoryCard;