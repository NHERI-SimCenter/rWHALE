
%Initial pick up of building file

formatSpec2 = '%d%d%d%f%d%f%f%f%f%f%f%f%f%f%C%f%f%C%f%f%C%C%f%f%f';
T = readtable('C:\Users\danie\Desktop\BuildingsRaw.csv', 'Delimiter', ',', ...
    'Format', formatSpec2);

%In order to gauge codes functionality I created a different truncated
%version of the initial table

T1 = T;
T1(200001:1843351,:) = [];

%Created new columns:
%Planar Area, Function, Building_Height, Replacement_Value,
%Structural_Type, General_Structural_Type, Code_for_Function,
%Code_for_Structural_Type, Code_for_General_Structural_Type

T1.Properties.VariableNames([17 18 19 20 21 22 23 24 25]) = {'Planar_Area' 'Function' 'Building_Height' 'Replacement_Value' 'Structural_Type' 'General_Structural_Type' 'Code_for_Function' 'Code_for_Structural_Type' 'Code_for_General_Structural_Type'};

% To solve for planar area it is assumed that all of the building are
% uniformily shaped throughout building height. 
% Takes total building squarefootage and divides by number of stories

T1.Planar_Area = T1.building_sqft./T1.stories;


%{
To determine building function UrbanSim's classification of building type and development type are both used.
Residential = Residential
Office, Hotel, School, Retail = Commercial
Industrial = Industrial
Unknown building type takes informationcconcerning development type to
narrow down the options then assigns remaining unknowns as Other. 

"building_type_map" (from urbansim example)
    1: "Residential",
    2: "Residential",
    3: "Residential",
    4: "Office",
    5: "Hotel",
    6: "School",
    7: "Industrial",
    8: "Industrial",
    9: "Industrial",
    10: "Retail",
    11: "Retail",
    12: "Residential",
    13: "Retail",
    14: "Office"

Developement Type:
(pg 303 from
https://astro.temple.edu/~jmennis/Courses/GUS_0150/readings/Waddell02.pdf)
1-8 Residential
9-16 Mixed R/C
17-19 Commercial
20-22 Industrial
23 - Government
24 - Vacant Developable 
25 - Undevelopable 

Assumption: 
1-12 Residential
13-19 Commercial
20-22 Industrial
23-25 Other
%}

for ii=1:size(T1.Function,1)
    
    %First tries to determine function from building_type_id provided from
    %UrbanSim
   if ~isempty(T1.building_type_id(ii)) && T1.building_type_id(ii) > 0
       if T1.building_type_id(ii) <= 3 || T1.building_type_id(ii) == 12
           T1.Function(ii) = 'Residential';
           T1.Code_for_Function(ii) = 1; %Used for graphing distribution later on
       elseif T1.building_type_id(ii) >= 7 && T1.building_type_id(ii) <= 9
           T1.Function(ii) = 'Industrial';
           T1.Code_for_Function(ii) = 3;
       else
           T1.Function(ii) = 'Commercial';
           T1.Code_for_Function(ii) = 2;
       end
    
    %If there is no building type or the building type is something other
    %than what has provided by UrbanSim, then try to determine building
    %function based on developement type
    
    elseif ~isempty(T1.development_type_id(ii)) 
        if T1.development_type_id(ii) <= 12 && T1.development_type_id(ii) > 0
            T1.Function(ii) = 'Residential';
            T1.Code_for_Function(ii) = 1;
        elseif T1.development_type_id(ii) > 12 && T1.development_type_id(ii) <= 19
            T1.Function(ii) = 'Commercial';
            T1.Code_for_Function(ii) = 2;
        elseif T1.development_type_id(ii) >= 20 && T1.development_type_id(ii) <= 22
            T1.Function(ii) = 'Industrial';
            T1.Code_for_Function(ii) = 3;
        else 
            T1.Function(ii) = 'Other';
            T1.Code_for_Function(ii) = 4;
        end
    else
        T1.Function(ii) = 'Other';
        T1.Code_for_Function(ii) = 4;
    end
end

%{
Assumption concerning Floor Heights:
Residential – 3.1 m
Commercial – 3.9 m 
Industrial – 4.5 m 
Other – Mixed 3.5 m 

Assumes building height is just the number of stories multiplied by the
corresponding floor height, obtained by assumption of functionality.
%}
for ii=1:size(T1.Building_Height,1)
    
    if T1.Function(ii) == 'Residential'
        T1.Building_Height(ii) = 3.1.*T1.stories(ii);
    elseif T1.Function(ii) == 'Commercial'
        T1.Building_Height(ii) = 3.9.*T1.stories(ii);
    elseif T1.Function(ii) == 'Industrial'
        T1.Building_Height(ii) = 4.5.*T1.stories(ii);
    else
        T1.Building_Height(ii) = 3.5.*T1.stories(ii);
    end 
end

%Replacement Value is assigned to the sale price is there is a sale price
%on record. If there is no sale price then the replacement value is
%assigned to the building squarefootage times the price per squarefoot. If
%there is no price per squarefoot then the replacement value is assigned to
%the building squarefootage times an arbitrary value (in this case I picked
%$400 but this might not be an accurate representation of the bay area)

for ii=1:size(T1.Replacement_Value,1)
    
    if ~isempty(T1.redfin_sale_price(ii)) && ~isnan(T1.redfin_sale_price(ii))
        T1.Replacement_Value(ii) = T1.redfin_sale_price(ii);
    elseif ~isempty(T1.res_price_per_sqft(ii)) && T1.res_price_per_sqft(ii) ~= 0
        T1.Replacement_Value(ii) = T1.building_sqft(ii).*T1.res_price_per_sqft(ii);
    else 
        T1.Replacement_Value(ii) = T1.building_sqft(ii).*400.00;
        % Assumption - $400 per sqarefoot is the general rate in the Bay Area
    end
end

%{
1 - URM – Unreinforced Masonry Bearing Walls
2 - RM1 – Reinforced Masonry Bearing Walls with Wood or Metal Deck Diaphragms
3 - RM2 – Reinforced Masonry Bearing Walls with Precast Concrete Diaphragms
4 - W1 -  Wood Frame
5 - S1 – Steel Moment Frame
6 - S2 – Steel Braced Frame
7 - C1 – Concrete Moment Frame
8 - C2 – Concrete Shear Walls
9 - C3 – Concrete Frame with Unreinforced Masonry Infill Walls
%}

for ii=1:size(T1.Code_for_Structural_Type,1)

    %First randomly assigns a structural type based on year of
    %construction, the building could be URM RM1 RM2 and W1
    if T1.year_built(ii) < 1901
        T1.Code_for_Structural_Type(ii) = randi([1 4],1,1);
        %Chooses randomly between:
        %URM RM1 RM2 W1
    elseif T1.stories(ii) <= 3
        if T1.Function(ii) == 'Residential'
            T1.Code_for_Structural_Type(ii) = 4;
            %W1
        elseif T1.Function(ii) == 'Commercial'
            T1.Code_for_Structural_Type(ii) = randi([2 9],1,1);
            %Chooses randomly between:
            %RM1 RM2 W1 S1 S2 C1 C2 C3
        elseif T1.Function(ii) == 'Industrial'
            T1.Code_for_Structural_Type(ii) = randi([5 9],1,1);
            %Chooses randomly between:
            %S1 S2 C1 C2 C3
        else 
            T1.Code_for_Structural_Type(ii) = 4;
            %W1
        end
    elseif T1.stories(ii) >= 4 && T1.stories(ii) <= 7
        if T1.Function(ii)== 'Industrial'
            T1.Code_for_Structural_Type(ii) = randi([5 8],1,1);
            %Chooses randomly between:
            %S1 S2 C1 C2
        else
            T1.Code_for_Structural_Type(ii) = randi([4 8],1,1);
            %Chooses randomly between:
            %W1 S1 S2 C1 C2
        end
    elseif T1.stories(ii) > 7
        T1.Code_for_Structural_Type(ii) = randi([5 8],1,1);
        %Chooses randomly between:
        %S1 S2 C1 C2
    else
        T1.Code_for_Structural_Type(ii) = randi([2 9],1,1);
        %Chooses randomly between:
        %RM1 RM2 W1 S1 S2 C1 C2 C3
    end
end

StructArr = ["URM" "RM1" "RM2" "W1" "S1" "S2" "C1" "C2" "C3"];
%This are used to graph the distribution of general structural type
GenStructArr = ["Masonry" "Masonry" "Masonry" "Wood-Frame" "Steel" "Steel" "Concrete" "Concrete" "Concrete"];
CodeGenStructArr = [8 8 8 5 7 7 6 6 6];
    
for ii=1:size(T1.Structural_Type,1)
    T1.Structural_Type(ii) = StructArr(T1.Code_for_Structural_Type(ii));
    T1.General_Structural_Type(ii) = GenStructArr(T1.Code_for_Structural_Type(ii));
    T1.Code_for_General_Structural_Type(ii) = CodeGenStructArr(T1.Code_for_Structural_Type(ii));
end
 
writetable(T1,'C:\Users\danie\Desktop\BuildingOut.csv')

