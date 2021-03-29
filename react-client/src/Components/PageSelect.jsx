import React, {useState, useEffect } from 'react';
import { Container, Row, Col } from 'react-bootstrap';

//This component is attached to a story page and renders the meta information of a story

function PageSelect(props) {
    function back(){
        let currentPage = parseInt(window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1));
        if (currentPage <= 1 || currentPage == NaN) return; //if you're already on the first page, you cant go back
        let pathBase = window.location.pathname.substring(0,window.location.pathname.lastIndexOf('/') + 1)
        currentPage-=1;
        props.dir(pathBase + currentPage.toString());
        props.shouldDir(true);
    }
    function forward(){
        let currentPage = parseInt(window.location.pathname.substring(window.location.pathname.lastIndexOf("/") + 1));
        let pathBase = "";
        if (!currentPage){ //if it doesn't have a page number in the url
            pathBase = window.location.pathname + "/";
            currentPage = 1;
        } else pathBase = window.location.pathname.substring(0,window.location.pathname.lastIndexOf('/') + 1)
        currentPage+=1;
        props.dir(pathBase + currentPage.toString()); //Page's setRedirect state function
        props.shouldDir(true);
    } 
    return(
        <Container className="pageSelect">
            <Row>
                <Col>
                    <input type="image" onClick={back} height="32px" src="/images/arrow_left.png"/>
                </Col>
                <Col>
                    <input type="image" onClick={forward} height="32px" src="/images/arrow_right.png"/>
                </Col>
            </Row>
        </Container>
    )
}
  
  export default PageSelect;