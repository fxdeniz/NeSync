addEventListener("DOMContentLoaded", (event) => {
    const buttonFileExplorer = document.getElementById("button-file-explorer");

    buttonFileExplorer.addEventListener('click', async clickEvent => {
        const contentContainer = document.getElementById("content-container");
        const htmlContent = await fetch('./resources/tabs/file_explorer.html').then((data) => data.text());
        contentContainer.innerHTML = htmlContent;
    });
});