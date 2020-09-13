import React, {useContext, useState, useEffect } from 'react';
import { AuthContext } from "../Context/Auth";
import { Container, Row, Col } from 'react-bootstrap';

function Rate(props){
    let [starsFilled, setStarsFilled] = useState([null]);//changes on mouseover
    let [starSelected, setStarSelected] = useState(null);
    let [userRating, setUserRating] = useState(props.meta.userRating);

    let auth = useContext(AuthContext);

    useEffect(() => {
        handleMoueLeave(); //to set the stars in the first place
      }, []);//only calls on mount

    function handleMoueLeave(){
        let ret = [];
        for (let i = 0; i < userRating; i+=5){
            ret[i/5] = true;
        }
        setStarsFilled(ret);
    }
    function handleClick(){
        let rating = (starSelected+1)*5; /*0-9 to 5-50*/
        setUserRating(rating);
        fetch(window.location.pathname + "?t=r&r=" +  rating, {
            method:"PUT", 
            headers: { Authorization: auth.authTokens.head},
        });
    }    

    function makeStars(){
        let starList = [];
        for (var i = 0; i < 10; i+=2){ //creates five stars, with even indices. The odds are for the right halves of the stars
            starList[i/2] = (
                <Star index={i} starsFilled={starsFilled} setStarsFilled={setStarsFilled} setStarSelected={setStarSelected} />
            )
        }
        return starList;
    }

    return(
    <Row className="stars" onMouseLeave={handleMoueLeave} onClick={handleClick}>
        {makeStars()}
    </Row>
    );
}


function Star(props){
    function handleMouseover(index){
        props.setStarSelected(index);
        var ret = [];
        for(var i = 0; i <= index; i++){
            ret[i] = true;
        }
        props.setStarsFilled(ret);
    }
    return(
    <div className= "star">
        <img onMouseEnter={() => handleMouseover(props.index)}
        height="24px" 
        src={props.starsFilled[props.index]?"/images/star_left_full.png":"/images/star_left_empty.png"} />

        <img onMouseEnter={() => handleMouseover(props.index+1)}
        height="24px" 
        src={props.starsFilled[props.index+1]?"/images/star_right_full.png":"/images/star_right_empty.png"} />
    </div>
    );
}

export default Rate;