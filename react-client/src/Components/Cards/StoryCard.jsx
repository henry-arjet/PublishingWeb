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
        return (
            <Card className="storyCard">
                <LinkContainer to = {"/" + this.props.link}>
                    <Card.Body>
                        <Card.Title>{this.props.title}</Card.Title>
                        <Container className="storyCardStats">
                            <Col>
                                <Row>
                                    <Card.Text as="Col">{this.props.time + " ago"}</Card.Text>
                                </Row>
                                <Row>
                                    <Card.Text>
                                        {this.props.rating != null ? this.props.rating + "/5.0  ": "Unrated  "}
                                        {this.props.views + " views"}
                                    </Card.Text>
                                </Row>
                            </Col>
                            <Col className = "flexR">
                                <Row className = "flexB">
                                    <LinkContainer to = {"/users/" + this.props.authorID}><Card.Link>{this.props.authorName}</Card.Link></LinkContainer>
                                    {this.props.words + " words"}
                                </Row>
                            </Col>
                        </Container>
                    </Card.Body>
                </LinkContainer>
            </Card>
        )
    }
}

export default StoryCard;