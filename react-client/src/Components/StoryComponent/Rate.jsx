import React, {useContext, useState, useEffect } from 'react';
import { AuthContext } from "../Context/Auth";
import { Container, Row, Col } from 'react-bootstrap';

function Rate(props){
    let [starsFilled, setStarsFilled] = useState([0, 0, 0, 0, 0]);//0 for empty, 1 for half, 2 for whole
    let [starSelected, setStarSelected] = useState(null);
    let [isConfirmed, setIsConfirmed] = useState(false); //props.meta.userRating !=0  once, ya know, meta.userRating exists



    return(
        <Container className="stars">
            <Row>
                <Star index={0} starsFilled={starsFilled} />
                <Star index={1} starsFilled={starsFilled} />
                <Star index={2} starsFilled={starsFilled} />
                <Star index={3} starsFilled={starsFilled} />
                <Star index={4} starsFilled={starsFilled} />
            </Row>
        </Container>
    );
}


function Star(props){

    return(
    <div className= "star">
        <img height="24px" src={props.starsFilled[props.index] != 0?"/images/star_left_whole.png":"/images/star_left_empty.png"} />
        <img height="24px" src={props.starsFilled[props.index] == 2?"/images/star_right_whole.png":"/images/star_right_empty.png"} />
    </div>
    );
}

export default Rate;