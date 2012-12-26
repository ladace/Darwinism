var current_conf = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
var changed_conf = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
var has_step = [false, false, false, false, false, false, false];
var is_walking = false;
var para_names = 
	["X-OFFSET",
	"Y-OFFSET",
	"Z-OFFSET",
	"ROLL-OFFSET",
	"PITCH-OFFSET",
	"YAW-OFFSET",
	"HIP-PITCH-OFFSET",
	"PERIOD-TIME",
	"DSP-RATIO",
	"STEP-FORWARDBACK-RATIO",
	"STEP-FORWARDBACK",
	"STEP-RIGHTLEFT",
	"STEP-DIRECTION",
	"FOOT-HEIGHT",
	"SWING-RIGHTLEFT",
	"SWING-TOPDOWN",
	"PELVIS-OFFSET",
	"ARM-SWING-GAIN"];
var engine_names =
	["R_SHOULDER_PITCH",
	"L_SHOULDER_PITCH",
	"R_SHOULDER_ROLL",
	"L_SHOULDER_ROLL",
	"R_ELBOW",
	"L_ELBOW",
	"R_HIP_YAW",
	"L_HIP_YAW",
	"R_HIP_ROLL",
	"L_HIP_ROLL",
	"R_HIP_PITCH",
	"L_HIP_PITCH",
	"R_KNEE",
	"L_KNEE",
	"R_ANKLE_PITCH",
	"L_ANKLE_PITCH",
	"R_ANKLE_ROLL",
	"L_ANKLE_ROLL",
	"HEAD_PAN",
	"HEAD_TILT"];
function reset_attrs(){
	changed_conf = current_conf.concat();
	for (i = 1; i < 19 ; i++){
		$("#para"+i+"_value").spinner("value", changed_conf[i-1]);
		$("#para"+i+"_slider").slider("value", changed_conf[i-1]);
	}
	$user_output = document.getElementById("user_output");
	update_output($user_output, "Configuration reset.");
	$.get("/walk/load-parset", function(data){
		current_conf = data.concat();
	});
}
function walk_switch(){
	is_walking = !is_walking;
	$walk_button = $("#walk_button");
	if (is_walking){
		$walk_button.button("option", "label", "Stop!");
		$system_output = document.getElementById("system_output");
		update_output($system_output, "Start walking.");
		$.get("/walk/start");
	} else{
		$walk_button.button("option", "label", "Walk!");
		$system_output = document.getElementById("system_output");
		update_output($system_output, "Stop walking.");
		$.get("/walk/stop");
	}
	
}
function send_attr_change(attr_name, attr_value){
	$.get("/walk/set-param", {param:attr_name, value:attr_value}, function(data){
		update_output(document.getElementById("system_output"), data);
	});
}
function add_conf(){
	$("#configuration_list").append("<li class='ui-widget-content'>Conf "+($("#configuration_list li").length+1)+"</li>");
	$("#configuration_list li.ui-selected").removeClass("ui-selected");
	$("#configuration_list li").last().addClass("ui-selected");
	$("#configuration_list").parent().scrollTop($("#configuration_list").parent().prop("scrollHeight"));
	$.get("/walk/save-new-parset");
}
function save_conf(){
	selected = $("#configuration_list .ui-selected");
	if (selected.length == 0) return;
	label = selected.html();
	$system_output = document.getElementById("system_output");
	update_output($system_output, "Save configuration to : "+label+".");
	$.get("/walk/save-parset", {id:label.split(" ")[1]});
	current_conf = changed_conf.concat();
}
function load_conf(conf_id){
	label = $("#configuration_list .ui-selected").html();
	$system_output = document.getElementById("system_output");
	update_output($system_output, "Load configuration from : "+label+".");
	$.get("/walk/load-parset", {id:conf_id}, function(data){
		changed_conf = current_conf = data.concat();
		update_output($system_output, data);
	}, "json");
}
function del_conf(){
	selected = $("#configuration_list .ui-selected");
	if (selected.length == 0) return;
	label = selected.html();
	$system_output = document.getElementById("system_output");
	update_output($system_output, "Configuration : "+label+" deleted.");
	selected.remove();
	$.get("/walk/del-parset", {id:label.split(" ")[1]});
	$("#configuration_list li").each(function(){
		$(this).html("Conf "+($(this).index()+1));
	});
}
function update_output(output_box, message){
	if (output_box.value == "")
		output_box.value = output_box.value + message;
	else
		output_box.value = output_box.value + "\n"+message;
	output_box.scrollTop = output_box.scrollHeight;
}