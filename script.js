"use strict";
let request = new XMLHttpRequest();
let data = {};
let front = document.querySelector("#front");
let background = document.querySelector("#background");
let currentIndex = 0;
let repeatable = true;
let images = [];

request.onreadystatechange = function () {
    if (request.readyState === 4 && request.status === 200) {
        data = JSON.parse(request.responseText);
        preloadImages().then();
        init();
    }
}
request.open('GET', `/assets.json`, true);
request.send();

async function preloadImages() {
    for (const file in data["images"]) {
        let filename = data["images"][file]["filename"];
        await loadImage(`/${filename}`).then(img => {
            images.push(img);
        });
    }
}

function loadImage(src) {
    return new Promise(function (resolve, reject) {
        let img = new Image();
        img.onload = function () {//加载时执行resolve函数
            resolve(img);
        }
        img.onerror = function () {
            reject(src);
        }
        img.src = src;
    });
}

// preloading
function init() {
    document.querySelector("#Loading").classList.add("hide");
    switchTo(0);
}

function totalImages() {
    return data["images"].length;
}

function switchTo(index) {
    if (repeatable) {
        if (index >= data["images"].length) index = 0;
        else if (index < 0) index = data["images"].length - 1;
    }
    console.assert(index >= 0 && index < data["images"].length);
    currentIndex = index;
    front.src = `/${data["images"][index]["filename"]}`;
    background.src = `/${data["images"][index]["filename"]}`;
    updateInfo();
    updateImageSize();
}

function updateInfo() {
    let title = document.querySelector("#imageName");
    let list = document.querySelector("#property");
    title.innerText = `/${data["images"][currentIndex]["filename"]}`;
    while (list.firstChild) {
        list.removeChild(list.firstChild);
    }
    for (let i = 0; i < data["images"][currentIndex]["category"].length; ++i) {
        list.appendChild(document.createElement("li"));
        list.lastChild.innerText = data["images"][currentIndex]["category"][i];

    }
}

function updateImageSize() {
    let windowHeight = window.innerHeight;
    let windowWidth = document.body.clientWidth;
    let currentImage = new Image();
    currentImage.src = `/${data["images"][currentIndex]["filename"]}`;
    front.width = Math.min(windowWidth, currentImage.width * windowHeight / currentImage.height);
}

document.onkeydown = function (e) {
    switch (e.key) {
        case "ArrowRight":
            switchTo(currentIndex + 1);
            break;
        case "ArrowLeft":
            switchTo(currentIndex - 1);
            break;
        case  "ArrowUp":
            switchTo(0);
            break;
        case  "ArrowDown":
            switchTo(totalImages() - 1);
            break;

        case "Enter":
            let shadow = document.querySelector("#shadow");
            if(shadow.classList.contains("hide"))shadow.classList.remove("hide");
            else shadow.classList.add("hide");

    }
}

window.onresize = function () {
    // if (timer) {
    //     clearTimeout(timer);
    // }
    // timer = setTimeout(() => {
    //     updateImageSize();
    // }, 200);
    updateImageSize();

}

// [fix me] incorrect size display
