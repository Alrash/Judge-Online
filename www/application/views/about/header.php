<?php
/**
 * Author: Alrash
 * 为about页面body中的整体布局
 */
?>
<div class="about" id="about">
    <!--导航栏-->
    <div class="left_area">
        <ul>
            <li>
                <a href="/about/suggest" title="<?php echo $about['left']['getSuggestTitle']?>"><?php echo $about['left']['getSuggest'];?></a>
            </li>
            <li>
                <a href="/about/help" title="<?php echo $about['left']['showHelpTitle'];?>"><?php echo $about['left']['showHelp'];?></a>
            </li>
            <li>
                <a href="/about/license" title="<?php echo $about['left']['licenseTitle'];?>"><?php echo $about['left']['license'];?></a>
            </li>
        </ul>
    </div>
    <div class="right_area">
