<script type="text/javascript" src="/js/question.js"></script>
<?php
/**
 * Author: Alrash
 */
if (!isset($pidExist) || is_null($pidExist) || !$pidExist){
    //pid is not existed
    //turn to index page
    echo "<script text='javascript'>window.location.href = '/question/index'</script>";
}else {
    echo "<div class='combination'>";
        echo "<div>";
            echo "<a href='/question/history/$pid' target='_blank'>" . $question["right"]["checkHistory"] . "</a>";
            echo "<a class='open-box'>". $question["right"]["submission"] ."</a>";
            echo "<a href='#'>" . $question["right"]['assist'] . "</a>";
        echo "</div>";
    echo "</div>";
    include 'submission.php';
    echo "<div class='pid'>";
        //first line, show title
        echo "<div class='pTitle'>";
            echo "<p>$questionTitle</p>";
        echo "</div>";
        //second line, show memory, time and hard mode
        echo "<div class='pInfo'>";
            echo "<ul>";
                echo "<li>" . $question['right']['content']['time'] . ": $time</li>";
                echo "<li>" . $question['right']['content']['memory'] . ": $memory</li>";
                echo "<li>" . $question['right']['content']['hardMode'] . ": $hardMode</li>";
            echo "</ul>";
        echo "</div>";
        //third line, show labels
        echo "<div class='pLabel'>";
            echo "<ul>";
            foreach ($labels as $num => $value){
                if(!is_null($value) && $value != null)
                    echo "<li>$value</li>";
            }
            echo "</ul>";
        echo "</div>";
        //show content of question
        //change background color
        echo "<div class='pContent'>";
            echo "<p>$content</p>";
        echo "</div>";
        //the last, show note like source or author
        echo "<div class='pNote'>" . $question['right']['content']['note'] . "<br>$note</div>";
    echo "</div>";
}